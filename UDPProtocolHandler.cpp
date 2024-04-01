/**
 * @file UDPProtocolHandler.cpp
 *
 * Implementation of the UDPProtocolHandler class
 *
 * @Author Marek Effenberger
 */

#include "UDPProtocolHandler.h"

// Send message to the port and address
void UDPProtocolHandler::send_message(const std::vector<uint8_t>& message) {
    if(sendto(sockfd, message.data(), message.size(), 0,(struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        clientOutput.internal_error_message("Error while sending the message");
    }
}

// Send confirm with messageID
void UDPProtocolHandler::send_confirmation(uint16_t messageID) {
    std::vector<uint8_t> response;
    response.push_back(0x00);
    response.push_back(static_cast<uint8_t>((messageID >> 8)));
    response.push_back(static_cast<uint8_t>(messageID & 0xFF));
    send_message(response);
}

// Resend the last message
void UDPProtocolHandler::resend_last_message() {
    // Resend the last message
    send_message(last_message);
}

// Check if the id is already in the received_ids
bool UDPProtocolHandler::id_lookup(uint16_t id){
    return std::any_of(received_ids.begin(), received_ids.end(), [id](uint16_t i) { return i == id; });
}

ProtocolHandler::ClientState UDPProtocolHandler::process_server_message() {
    // Receive the message
    // Receive the message from the server
    char buffer[1500];
    struct sockaddr_in fromAddr; // Temporary storage for the sender's address
    socklen_t fromLen = sizeof(fromAddr);

    ssize_t msgLen = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&fromAddr, &fromLen);
    if (msgLen < 0) {
        // Handle error
        perror("recvfrom failed");
        return ProtocolHandler::ClientState::OVER; // Example error state
    }
    // Set the server address on the new message
    memcpy(&server_address, &fromAddr, sizeof(fromAddr));

    std::vector<uint8_t> message(buffer, buffer + msgLen);

    // State can stay WAITING_FOR_CONFIRMATION if the confirmation is not received
    ProtocolHandler::ClientState state = process_received(message);
    return waiting_for_confirmation ? ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION : state;
}

void UDPProtocolHandler::send_error_message(const std::string& error) {
    // If waiting for confirmation the error shall be printed after the confirmation
    if(waiting_for_confirmation){
        error_message = error;
        error_while_waiting = true;
        return;
    }
    std::vector<uint8_t> response = messageValidator.form_error_message(renamer3000, messageValidator.get_display_name(), error);
    send_message(response);
    awaited_conirm_id = renamer3000;
    last_message = response;
    error_sent = true;
    renamer3000++;
    waiting_for_confirmation = true;
}


// Process the message from the server
ProtocolHandler::ClientState UDPProtocolHandler::process_received(const std::vector<uint8_t>& message) {
    // First byte of the message represents the type of the message
    uint8_t first_byte = message[0];

    // CONFIRMATION
    if (first_byte == 0x00){
        // Confirmation message
        uint16_t messageID = (message[1] << 8) | message[2];


        if (messageID == awaited_conirm_id){
            awaited_conirm_id = -1;

            // Awaiting over - confirmation for the bye sent, the connection is over
            if (awaiting_over){
                waiting_for_confirmation = false;
                return ProtocolHandler::ClientState::OVER;
            }

            // Awaiting confirmation for the error message, send bye
            if (error_sent){
                error_sent = false;
                std::vector<uint8_t> response = messageValidator.form_bye_message(renamer3000);
                send_message(response);
                awaited_conirm_id = renamer3000;
                last_message = response;
                awaiting_over = true;
                renamer3000++;
                return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
            }

            // Awaiting confirmation for the reply
            if (waiting_for_reply){
                waiting_for_reply = false;
                waiting_for_confirmation = false;
                return ProtocolHandler::ClientState::WAITING_FOR_REPLY;
            }
            // Else, ready for input
            waiting_for_confirmation = false;
            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
        return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
    }

    // REPLY
    else if (first_byte == 0x01){
        //Reply message
        if (message.size() < 6){
            send_error_message("Unexpected reply message format");
            return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
        }
        // States for the FSM
        FSMValidate::Action action;
        FSMValidate::Action reaction;

        // Parsing the message
        uint16_t messageID = (message[1] << 8) | message[2];
        uint8_t status = message[3];
        uint16_t refID = (message[4] << 8) | message[5];
        // content shall be from 6 to the zero byte
        std::vector<uint8_t> content(message.begin() + 6, message.end());
        auto [response, success] = messageValidator.parse_and_validate(content, "reply");
        // If the content is valid
        if (success) {

            // 0x00 NREPLY
            if (status == 0x00) {

                action = FSMValidate::Action::NREPLY_SERVER;
                reaction = fsm.validate_action(action);

                // If FSM allows the action
                if (reaction == FSMValidate::Action::ANY) {
                    // If the reply is awaited
                    if (refID == awaited_reply_id) {
                        clientOutput.reply_error(response[0]);
                        send_confirmation(messageID);
                        received_ids.push_back(messageID);
                        waiting_for_reply = false;
                        awaited_reply_id = -1;
                        return ProtocolHandler::ClientState::READY_FOR_INPUT;
                    } else {
                        // Ignore but send conf
                        send_confirmation(messageID);
                        return ProtocolHandler::ClientState::WAITING_FOR_REPLY;
                    }

                } else if (reaction == FSMValidate::Action::ERROR_USER){
                    // Unexpected state for the negative reply
                    send_confirmation(messageID);
                    send_error_message("Invalid state for negative reply message");
                    return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
                }
            }
            // REPLY
            else if (status == 0x01) {
                action = FSMValidate::Action::REPLY_SERVER;
                reaction = fsm.validate_action(action);


                // If FSM allows the action
                if (reaction == FSMValidate::Action::ANY) {
                    // If the reply is awaited
                    if (refID == awaited_reply_id) {
                        clientOutput.reply_success(response[0]);
                        send_confirmation(messageID);
                        received_ids.push_back(messageID);
                        waiting_for_reply = false;
                        awaited_reply_id = -1;
                        return ProtocolHandler::ClientState::READY_FOR_INPUT;
                    } else {
                        // Ignore but send conf
                        send_confirmation(messageID);
                        return ProtocolHandler::ClientState::READY_FOR_INPUT;
                    }
                } else if (reaction == FSMValidate::Action::ERROR_USER){
                    // Unexpected state for the positive reply
                    send_confirmation(messageID);
                    send_error_message("Invalid state for positive reply message");
                    return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
                }

            }

            else {
                // Error, invalid nok/ok
                send_confirmation(messageID);
                send_error_message("Invalid reply message format");
                return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;

            }

        }
            // Invalid content received, too short or invalid
            else {
                // ERROR SERVER SIDE
                send_error_message("Invalid content in reply message");
                return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
        }
    }

    // MESSAGE
    else if (first_byte == 0x04){

        if (message.size() < 3){
            send_error_message("Unexpected message format");
            return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
        }

        FSMValidate::Action action;
        FSMValidate::Action reaction;
        // Parsing the message
        uint16_t messageID = (message[1] << 8) | message[2];

        std::vector<uint8_t> content(message.begin() + 3, message.end());
        auto [response, success] = messageValidator.parse_and_validate(content, "msg");
        // If the content is valid
        if (success){
            action = FSMValidate::Action::MESSAGE_SERVER;
            reaction = fsm.validate_action(action);
            if (reaction == FSMValidate::Action::ANY) {
                // If the message has not yet been received print it, otherwise just confirm
                if (id_lookup(messageID)) {
                    send_confirmation(messageID);
                    return ProtocolHandler::ClientState::READY_FOR_INPUT;
                } else {
                    send_confirmation(messageID);
                    received_ids.push_back(messageID);
                    clientOutput.message_from_server(response[0], response[1]);
                    return ProtocolHandler::ClientState::READY_FOR_INPUT;
                }

            } else {
                // Error, invalid state for the message
                send_error_message("Invalid state for message message");
                return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
            }
        } else  {
            // Error, invalid content in the message
            send_error_message("Invalid content in message");
            return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
        }
    }

    // ERR
    else if (first_byte == 0xFE) {
        // Err message
        if (message.size() < 4){
            send_error_message("Unexpected error message format");
            return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
        }
        FSMValidate::Action action;
        FSMValidate::Action reaction;
        // Parsing the message
        uint16_t messageID = (message[1] << 8) | message[2];
        auto [response, success] = messageValidator.parse_and_validate(std::vector<uint8_t>(message.begin() + 3, message.end()), "err");
        // If the content is valid
        if (success){

            action = FSMValidate::Action::ERROR_SERVER;
            reaction = fsm.validate_action(action);

            // If the FSM allows the action
            if (reaction == FSMValidate::Action::ANY) {
                // If the message has not yet been received print it, otherwise just confirm
                if (id_lookup(messageID)) {
                    send_confirmation(messageID);
                    // send bye
                    std::vector<uint8_t> response = messageValidator.form_bye_message(renamer3000);
                    send_message(response);
                    awaited_conirm_id = renamer3000;
                    last_message = response;
                    awaiting_over = true;
                    renamer3000++;
                    waiting_for_confirmation = true;
                    return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
                } else {
                    send_confirmation(messageID);
                    received_ids.push_back(messageID);
                    clientOutput.error_from_server(response[0], response[1]);

                    std::vector<uint8_t> response = messageValidator.form_bye_message(renamer3000);
                    send_message(response);
                    awaited_conirm_id = renamer3000;
                    last_message = response;
                    awaiting_over = true;
                    renamer3000++;
                    return ProtocolHandler::ClientState::READY_FOR_INPUT;
                }
            } else {
                // Error, invalid state for the error message
                send_confirmation(messageID);
                send_error_message("Invalid state for error message");
                return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
            }
        } else {
            // Error, invalid content in the error message
            send_confirmation(messageID);
            send_error_message("Invalid content in error message");
            return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
        }

    }

    // BYE
    else if (first_byte == 0xFF) {
        //bye message
        if (message.size() < 3){
            send_error_message("Unexpected bye message format");
            return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
        }
        uint16_t messageID = (message[1] << 8) | message[2];
        received_ids.push_back(messageID);
        send_confirmation(messageID);
        return ProtocolHandler::ClientState::OVER;

    } else {
        // Error, unexpected message
        if (message.size() < 3){
            send_error_message("Unexpected format");
            return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
        }
        uint16_t messageID = (message[1] << 8) | message[2];
        send_confirmation(messageID);
        send_error_message("Unexpected format");
        return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
    }
}


// Process the user input
ProtocolHandler::ClientState UDPProtocolHandler::process_user_input(const std::string& message) {
    // My specific command, just sends bye and waits for the confirmation
    if (message == "/exit"){
        std::vector<uint8_t> response = messageValidator.form_bye_message(renamer3000);
        send_message(response);
        last_message = response;
        awaiting_over = true;
        awaited_conirm_id = renamer3000;
        renamer3000++;
        waiting_for_confirmation = true;

        return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
    }

    // If the error was sent while waiting for confirmation, print it and return
    if (error_while_waiting){
        send_error_message(error_message);
        return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
    }

    // Split the message by space
    size_t pos = message.find(' ');
    std::string command = message.substr(0, pos);
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    std::string argument = message.substr(pos + 1);


    FSMValidate::Action action;
    FSMValidate::Action reaction;
    // Get the display name
    std::string Username = messageValidator.get_display_name();

    // Switch based on the command
    if (command == "/AUTH"){

        action = FSMValidate::Action::AUTHORIZE_USER;
        reaction = fsm.validate_action(action);
        // If the FSM allows the action
        if (reaction == FSMValidate::Action::ANY){
            std::vector<std::string> parts = messageValidator.split_message(argument);
            if (parts.size() != 3){
                clientOutput.internal_error_message("Invalid message format");
                return ProtocolHandler::ClientState::READY_FOR_INPUT;
            }
            // The format must be shuffled because of the specification
            auto [response, success] = messageValidator.authorize_validate(0x02, renamer3000, parts[0], parts[2], parts[1]);
            // Based on the bool we either send it or print it to the user
            if (success){
                send_message(response);
                waiting_for_reply = true;
                last_message = response;

                awaited_reply_id = renamer3000;
                awaited_conirm_id = renamer3000;
                renamer3000++;
                waiting_for_confirmation = true;
                return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
            }
            // Error, invalid format
            else{
                std::string str(response.begin(), response.end());
                clientOutput.internal_error_message(str);
                return ProtocolHandler::ClientState::READY_FOR_INPUT;
            }
        }
        // Error, invalid state for the authorize
        else if (reaction == FSMValidate::Action::WARN_CLIENT){
            clientOutput.internal_error_message("You are not authorized to authorize at this state!");
            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
    }
    // JOIN
    else if (command == "/JOIN"){
        action = FSMValidate::Action::JOIN_USER;
        reaction = fsm.validate_action(action);
        // If the FSM allows the action
        if (reaction == FSMValidate::Action::ANY){
            std::vector<std::string> parts = messageValidator.split_message(argument);
            if (parts.size() != 1){
                clientOutput.internal_error_message("Invalid message format");
                return ProtocolHandler::ClientState::READY_FOR_INPUT;
            }
            auto [response, success] = messageValidator.join_validate(0x03, renamer3000, parts[0]);

            // Based on the bool we either send it or print it to the user
            if (success){
                send_message(response);
                waiting_for_reply = true;
                last_message = response;

                awaited_reply_id = renamer3000;
                awaited_conirm_id = renamer3000;
                renamer3000++;
                waiting_for_confirmation = true;
                return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
            }
            // Error, invalid format
            else{
                std::string str(response.begin(), response.end());
                clientOutput.internal_error_message(str);
                return ProtocolHandler::ClientState::READY_FOR_INPUT;
            }
        }
        // Error, invalid state for the join
        else if (reaction == FSMValidate::Action::WARN_CLIENT){
            clientOutput.internal_error_message("You are not authorized to join at this state!");
            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
    }
    // RENAME
    else if (command == "/RENAME"){
        if (!(messageValidator.rename(argument))){
            clientOutput.internal_error_message("Invalid display name format");
            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
    }
    // HELP
    else if (command == "/HELP"){
        clientOutput.print_help();
        return ProtocolHandler::ClientState::READY_FOR_INPUT;
    }
    // MESSAGE
    else{
        // if first byte / - error
        if (message[0] == '/'){
            clientOutput.internal_error_message("Invalid command");
            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
        action = FSMValidate::Action::MESSAGE_USER;
        reaction = fsm.validate_action(action);
        // If the FSM allows the action
        if (reaction == FSMValidate::Action::ANY){
            // If the message is empty
            auto [response, success] = messageValidator.message_validate(0x04, renamer3000, message);
            // Based on the bool we either send it or print it to the user
            if (success){
                send_message(response);

                waiting_for_reply = false;
                last_message = response;

                awaited_reply_id = -1;
                awaited_conirm_id = renamer3000;
                renamer3000++;
                waiting_for_confirmation = true;
                return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
            }
            else{
                std::string str(response.begin(), response.end());
                clientOutput.internal_error_message(str);
                return ProtocolHandler::ClientState::READY_FOR_INPUT;
            }
        }
        // Error, invalid state for the message
        else if (reaction == FSMValidate::Action::WARN_CLIENT){
            clientOutput.internal_error_message("You are not authorized to send a message at this state!");
            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
    }
}
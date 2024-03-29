//
// Created by marek on 26.03.2024.
//

#include "UDPProtocolHandler.h"

void UDPProtocolHandler::send_message(const std::vector<uint8_t>& message) {
    if(sendto(sockfd, message.data(), message.size(), 0,(struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        clientOutput.internal_error_message("Error while sending the message");
    }
}

void UDPProtocolHandler::send_confirmation(uint16_t messageID) {
    std::vector<uint8_t> response;
    //first byte shall be 0x00
    response.push_back(0x00);
    //second two bytes shall be the messageID
    response.push_back((messageID >> 8) & 0xFF);
    send_message(response);
}

void UDPProtocolHandler::resend_last_message() {
    // Resend the last message
    send_message(last_message);
}

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

    memcpy(&server_address, &fromAddr, sizeof(fromAddr));


    std::vector<uint8_t> message(buffer, buffer + msgLen);

    return process_received(message);

}

ProtocolHandler::ClientState UDPProtocolHandler::process_received(const std::vector<uint8_t>& message) {
    // Process the message
    // Process the message from the server
    // based on the protocol and the Mealy machine logic
    uint8_t first_byte = message[0];

    // CONFIRMATION
    if (first_byte == 0x00){
        //confirmation message
        uint16_t messageID = (message[1] << 8) | message[2];

        if (messageID == awaited_conirm_id){

            printf("Confirmation received\n");
            awaited_conirm_id = 0;
            if (awaiting_over){
                return ProtocolHandler::ClientState::OVER;
            }

            if (error_sent){
                error_sent = false;
                std::vector<uint8_t> response = messageValidator.form_bye_message(++renamer3000);
                send_message(response);
                awaited_conirm_id = renamer3000;
                last_message = response;
                awaiting_over = true;
                return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
            }

            if (waiting_for_reply){
                return ProtocolHandler::ClientState::WAITING_FOR_REPLY;
            }

            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
        return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
    }

    // REPLY
    else if (first_byte == 0x01){
        //reply message
        if (message.size() < 6){
            std::vector<uint8_t> response = messageValidator.form_error_message(++renamer3000, messageValidator.get_display_name(), "Unexpected reply message format");
            send_message(response);
            awaited_conirm_id = renamer3000;
            last_message = response;
            error_sent = true;
            return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
        }

        FSMValidate::Action action;
        FSMValidate::Action reaction;

        uint16_t messageID = (message[1] << 8) | message[2];
        uint8_t status = message[3];
        uint16_t refID = (message[4] << 8) | message[5];
        // content shall be from 6 to the zero byte
        std::vector<uint8_t> content(message.begin() + 6, message.end());
        auto [response, success] = messageValidator.parse_and_validate(content, "reply");

            if (success) {

                // 0x00 NREPLY
                if (status == 0x00) {

                    action = FSMValidate::Action::NREPLY_SERVER;
                    reaction = fsm.validate_action(action);
                    if (reaction == FSMValidate::Action::ANY) {

                        if (refID == awaited_reply_id) {
                            clientOutput.reply_error(response[1]);
                            send_confirmation(messageID);
                            received_ids.push_back(messageID);
                            awaited_reply_id = 0;
                            return ProtocolHandler::ClientState::READY_FOR_INPUT;
                        } else {
                            // IGNORE WE BALLIN but send conf
                            send_confirmation(messageID);
                            return ProtocolHandler::ClientState::WAITING_FOR_REPLY;
                        }

                    } else if (reaction == FSMValidate::Action::ERROR_USER){

                        // ERROR SEND TO SERVER
                        std::vector<uint8_t> response = messageValidator.form_error_message(++renamer3000, messageValidator.get_display_name(), "Invalid reply message format");
                        send_message(response);
                        awaited_conirm_id = renamer3000;
                        last_message = response;
                        error_sent = true;
                        return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
                    }
                }
                    // REPLY
                else if (status == 0x01) {
                    action = FSMValidate::Action::REPLY_SERVER;
                    reaction = fsm.validate_action(action);
                    if (reaction == FSMValidate::Action::ANY) {

                        if (refID == awaited_reply_id) {
                            clientOutput.reply_success(response[1]);
                            send_confirmation(messageID);
                            received_ids.push_back(messageID);
                            awaited_reply_id = 0;
                            return ProtocolHandler::ClientState::READY_FOR_INPUT;
                        } else {
                            // IGNORE WE BALLIN but send conf
                            send_confirmation(messageID);
                            return ProtocolHandler::ClientState::READY_FOR_INPUT;
                        }
                    } else if (reaction == FSMValidate::Action::ERROR_USER){

                        // ERROR SEND TO SERVER
                        std::vector<uint8_t> response = messageValidator.form_error_message(++renamer3000, messageValidator.get_display_name(), "Invalid reply message format");
                        send_message(response);
                        awaited_conirm_id = renamer3000;
                        last_message = response;
                        error_sent = true;
                        return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
                    }

                }

                else {
                    // ERROR SERVER SIDE, NOK OR OK NOT RECEIVED
                    std::vector<uint8_t> response = messageValidator.form_error_message(++renamer3000, messageValidator.get_display_name(), "Invalid reply message format");
                    send_message(response);
                    awaited_conirm_id = renamer3000;
                    last_message = response;
                    error_sent = true;
                    return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;

                }

            }
                // Invalid content received
                else {
                    // ERROR SERVER SIDE
                    std::vector<uint8_t> err = messageValidator.form_error_message(++renamer3000, messageValidator.get_display_name(), response[0]);
                    send_message(err);
                    awaited_conirm_id = renamer3000;
                    last_message = err;
                    error_sent = true;
                    return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
            }
    }

    // MESSAGE
    else if (first_byte == 0x04){
        //message message

        if (message.size() < 3){
            ////////////////////////////////////ERROR SERVER SIDE////////////////////////////////////////
            std::vector<uint8_t> response = messageValidator.form_error_message(++renamer3000, messageValidator.get_display_name(), "Unexpected message format");
            send_message(response);
            awaited_conirm_id = renamer3000;
            last_message = response;
            error_sent = true;
            return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
        }

        FSMValidate::Action action;
        FSMValidate::Action reaction;

        uint16_t messageID = (message[1] << 8) | message[2];
        std::vector<uint8_t> content(message.begin() + 3, message.end());
        auto [response, success] = messageValidator.parse_and_validate(content, "message");

        if (success){
            action = FSMValidate::Action::MESSAGE_SERVER;
            reaction = fsm.validate_action(action);
            if (reaction == FSMValidate::Action::ANY) {

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
                // ERROR SERVER SIDE
                std::vector<uint8_t> response = messageValidator.form_error_message(++renamer3000, messageValidator.get_display_name(), "Unexpected Message");
                send_message(response);
                awaited_conirm_id = renamer3000;
                last_message = response;
                error_sent = true;
                return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
            }
        } else  {
            // ERROR SERVER SIDE
            std::vector<uint8_t> err = messageValidator.form_error_message(++renamer3000, messageValidator.get_display_name(), response[0]);
            send_message(err);
            awaited_conirm_id = renamer3000;
            last_message = err;
            error_sent = true;
            return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
        }
    }

    // ERR
    else if (first_byte == 0xFE) {
        //err message
        if (message.size() < 4){
            std::vector<uint8_t> response = messageValidator.form_error_message(++renamer3000, messageValidator.get_display_name(), "Unexpected error message format");
            send_message(response);
            awaited_conirm_id = renamer3000;
            last_message = response;
            error_sent = true;
            return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
        }
        FSMValidate::Action action;
        FSMValidate::Action reaction;

        uint16_t messageID = (message[1] << 8) | message[2];
        auto [response, success] = messageValidator.parse_and_validate(std::vector<uint8_t>(message.begin() + 3, message.end()), "error");

        if (success){
            action = FSMValidate::Action::ERROR_SERVER;
            reaction = fsm.validate_action(action);
            if (reaction == FSMValidate::Action::ANY) {
                if (id_lookup(messageID)) {
                    send_confirmation(messageID);

                    // send bye
                    std::vector<uint8_t> response = messageValidator.form_bye_message(++renamer3000);
                    send_message(response);
                    awaited_conirm_id = renamer3000;
                    last_message = response;
                    awaiting_over = true;
                    return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
                } else {
                    send_confirmation(messageID);
                    received_ids.push_back(messageID);
                    clientOutput.error_from_server(response[0], response[1]);

                    std::vector<uint8_t> response = messageValidator.form_bye_message(++renamer3000);
                    send_message(response);
                    awaited_conirm_id = renamer3000;
                    last_message = response;
                    awaiting_over = true;
                    return ProtocolHandler::ClientState::READY_FOR_INPUT;
                }
            } else {
                // ERROR SERVER SIDE
                std::vector<uint8_t> response = messageValidator.form_error_message(++renamer3000, messageValidator.get_display_name(), "Unexpected Error");
                send_message(response);
                awaited_conirm_id = renamer3000;
                last_message = response;
                error_sent = true;
                return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
            }
        } else {
            // ERROR SERVER SIDE
            std::vector<uint8_t> err = messageValidator.form_error_message(++renamer3000, messageValidator.get_display_name(), response[0]);
            send_message(err);
            awaited_conirm_id = renamer3000;
            last_message = err;
            error_sent = true;
            return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
        }

    }

    // BYE
    else if (first_byte == 0xFF) {
        //bye message
        uint16_t messageID = (message[1] << 8) | message[2];
        received_ids.push_back(messageID);
        send_confirmation(messageID);
        return ProtocolHandler::ClientState::OVER;

    } else {
        // ERROR SERVER SIDE
        std::vector<uint8_t> response = messageValidator.form_error_message(++renamer3000,
                                                                            messageValidator.get_display_name(),
                                                                            "Unexpected message format");
        send_message(response);
        awaited_conirm_id = renamer3000;
        last_message = response;
        error_sent = true;
        return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
    }

}

ProtocolHandler::ClientState UDPProtocolHandler::process_user_input(const std::string& message) {
    // Send the message
    // Send the message to the server
    // based on the protocol and the Mealy machine logic
    if (message == "/exit"){
        std::vector<uint8_t> response = messageValidator.form_bye_message(++renamer3000);
        send_message(response);
        last_message = response;
        awaiting_over = true;
        return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
    }


    size_t pos = message.find(' ');
    std::string command = message.substr(0, pos);
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    std::string argument = message.substr(pos + 1);
    std::transform(argument.begin(), argument.end(), argument.begin(), ::toupper);



    FSMValidate::Action action;
    FSMValidate::Action reaction;

    std::string Username = messageValidator.get_display_name();

    //switch based on the command
    if (command == "/AUTH"){


        action = FSMValidate::Action::AUTHORIZE_USER;
        reaction = fsm.validate_action(action);


        if (reaction == FSMValidate::Action::ANY){

            std::vector<std::string> parts = messageValidator.split_message(argument);
            if (parts.size() != 3){
                clientOutput.internal_error_message("Invalid message format");
                return ProtocolHandler::ClientState::READY_FOR_INPUT;
            }

            auto [response, success] = messageValidator.authorize_validate(0x02, ++renamer3000, parts[0], parts[2], parts[1]);
            //based on the bool we either send it or print it to the user

            if (success){

                send_message(response);
                waiting_for_reply = true;
                last_message = response;

                awaited_reply_id = renamer3000;
                awaited_conirm_id = renamer3000;

                ////////////////////////////////////////////////////////////////////////////////////////////////////
                return ProtocolHandler::ClientState::WAITING_FOR_REPLY;
                ////////////////////////////////////////////////////////////////////////////////////////////////////
            }
            else{
                renamer3000--;
                std::string str(response.begin(), response.end());
                clientOutput.internal_error_message(str);
                return ProtocolHandler::ClientState::READY_FOR_INPUT;
            }
        }
        else if (reaction == FSMValidate::Action::WARN_CLIENT){
            clientOutput.internal_error_message("You are not authorized to authorize at this state!");
            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
    }

    else if (command == "/JOIN"){
        action = FSMValidate::Action::JOIN_USER;
        reaction = fsm.validate_action(action);

        if (reaction == FSMValidate::Action::ANY){

            std::vector<std::string> parts = messageValidator.split_message(argument);
            if (parts.size() != 1){
                clientOutput.internal_error_message("Invalid message format");
                return ProtocolHandler::ClientState::READY_FOR_INPUT;
            }
            auto [response, success] = messageValidator.join_validate(0x03, ++renamer3000, parts[0]);

            //based on the bool we either send it or print it to the user
            if (success){

                send_message(response);
                waiting_for_reply = true;
                last_message = response;

                awaited_reply_id = renamer3000;
                awaited_conirm_id = renamer3000;

                ////////////////////////////////////////////////////////////////////////////////////////////////////
                return ProtocolHandler::ClientState::WAITING_FOR_REPLY;
                ////////////////////////////////////////////////////////////////////////////////////////////////////
            }
            else{
                renamer3000--;
                std::string str(response.begin(), response.end());
                clientOutput.internal_error_message(str);
                return ProtocolHandler::ClientState::READY_FOR_INPUT;
            }
        }
        else if (reaction == FSMValidate::Action::WARN_CLIENT){
            clientOutput.internal_error_message("You are not authorized to join at this state!");
            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
    }

    else if (command == "/RENAME"){

        if (!(messageValidator.rename(argument))){
            clientOutput.internal_error_message("Invalid display name format");
            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
    }

    else if (command == "/HELP"){
        clientOutput.print_help();
        return ProtocolHandler::ClientState::READY_FOR_INPUT;
    }

    else{
        action = FSMValidate::Action::MESSAGE_USER;
        reaction = fsm.validate_action(action);

        if (reaction == FSMValidate::Action::ANY){

            auto [response, success] = messageValidator.message_validate(0x04, ++renamer3000, message);

            if (success){
                send_message(response);

                waiting_for_reply = false;
                last_message = response;

                awaited_reply_id = 0;
                awaited_conirm_id = renamer3000;
                return ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
            }
            else{
                renamer3000--;
                std::string str(response.begin(), response.end());
                clientOutput.internal_error_message(str);
                return ProtocolHandler::ClientState::READY_FOR_INPUT;
            }
        }
        else if (reaction == FSMValidate::Action::WARN_CLIENT){
            clientOutput.internal_error_message("You are not authorized to send a message at this state!");
            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
    }
}
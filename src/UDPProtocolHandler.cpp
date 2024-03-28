//
// Created by marek on 26.03.2024.
//

#include "UDPProtocolHandler.h"

void UDPProtocolHandler::send_message(const std::vector<uint8_t>& message) {
    if(sendto(sockfd, message.data(), message.size(), 0,(struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        clientOutput.internal_error_message("Error while sending the message");
    }
}

void UDPProtocolHandler::resend_last_message() {
    // Resend the last message
    return;
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

    return ProtocolHandler::ClientState::WAITING_FOR_REPLY;
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
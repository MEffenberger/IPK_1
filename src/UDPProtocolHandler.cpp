//
// Created by marek on 26.03.2024.
//

#include "UDPProtocolHandler.h"

void UDPProtocolHandler::send_message(const std::string& message) {
    // Send the message
    if(sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        clientOutput.internal_error_message("Error while sending the message");
        //possibly exit i dunno
    }
}

void UDPProtocolHandler::resend_last_message() {
    // Resend the last message
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
        return ProtocolHandler::ClientState::ERROR; // Example error state
    }

    memcpy(&server_address, &fromAddr, sizeof(fromAddr));

    return ProtocolHandler::ClientState::WAITING_FOR_REPLY;
}

ProtocolHandler::ClientState UDPProtocolHandler::process_user_input(const std::string& message) {
    // Send the message
    // Send the message to the server
    // based on the protocol and the Mealy machine logic
    if (message == "/exit"){

        send_message("BYE\r\n");

        return ProtocolHandler::ClientState::OVER;
    }


    size_t pos = message.find(' ');
    std::string command = message.substr(0, pos);
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    std::string argument = message.substr(pos + 1);
    std::transform(argument.begin(), argument.end(), argument.begin(), ::toupper);


    std::string response;
    FSMValidate::Action action;
    FSMValidate::Action reaction;

    std::string Username = messageValidator.getDisplayName();

    //switch based on the command
    if (command == "/AUTH"){


        action = FSMValidate::Action::AUTHORIZE_USER;
        reaction = fsm.validate_action(action);


        if (reaction == FSMValidate::Action::ANY){
            response = messageValidator.authorize_validate(argument).first;
            //based on the bool we either send it or print it to the user
            if (messageValidator.authorize_validate(argument).second){
                send_message(response);
                waiting_for_reply = true;
                return ProtocolHandler::ClientState::WAITING_FOR_REPLY;
            }
            else{
                clientOutput.internal_error_message(response);
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
            response = messageValidator.join_validate(argument).first;
            //based on the bool we either send it or print it to the user
            if (messageValidator.join_validate(argument).second){
                send_message(response);
                waiting_for_reply = true;
                return ProtocolHandler::ClientState::WAITING_FOR_REPLY;
            }
            else{
                clientOutput.internal_error_message(response);
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

    else {
        action = FSMValidate::Action::MESSAGE_USER;
        reaction = fsm.validate_action(action);

        if (reaction == FSMValidate::Action::ANY){
            response = messageValidator.message_validate(message).first;
            if (messageValidator.message_validate(message).second){
                send_message(response);
            }
            else{
                clientOutput.internal_error_message(response);
            }

        }
        else if (reaction == FSMValidate::Action::WARN_CLIENT){
            clientOutput.internal_error_message("You are not authorized to send a message at this state!");
        }
        return ProtocolHandler::ClientState::READY_FOR_INPUT;
    }
}
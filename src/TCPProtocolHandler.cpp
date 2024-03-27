//
// Created by marek on 16.03.2024.
//

#include "TCPProtocolHandler.h"


void TCPProtocolHandler::send_message(const std::string& message) {
    // Send the message to the server
    // based on the protocol and the Mealy machine logic
    if (send(sockfd, message.c_str(), message.size(), 0) == -1) {
        std::cerr << "Error sending message to the server" << std::endl;
        exit(EXIT_FAILURE);
    }
}


ProtocolHandler::ClientState TCPProtocolHandler::process_user_input(const std::string& message) {
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


ProtocolHandler::ClientState TCPProtocolHandler::process_server_message() {
    static std::string buffer; // Static to retain data across calls
    char recvBuffer[1500];
    ssize_t bytes_received;

    while (true) {
        memset(recvBuffer, 0, sizeof(recvBuffer));
        bytes_received = recv(sockfd, recvBuffer, sizeof(recvBuffer) - 1, 0);

        if (bytes_received < 0) {
            perror("recv");
            // Handle error, potentially return a specific state or throw
        } else if (bytes_received == 0) {
            std::cout << "Server closed the connection" << std::endl;
            close(sockfd);
            return ProtocolHandler::ClientState::OVER;
        } else {
            // Successfully received data, append to the buffer
            buffer.append(recvBuffer, bytes_received);

            // Process all complete messages in the buffer
            size_t pos;
            while ((pos = buffer.find("\r\n")) != std::string::npos) {
                std::string message = buffer.substr(0, pos); // Extract the message
                buffer.erase(0, pos + 2); // Remove the processed message from the buffer \r\nmsg from

                // Process the extracted message
                auto state = process_received(message);
                if (state != ProtocolHandler::ClientState::READY_FOR_INPUT) {
                    return state;
                }
            }

            // If we reach here, either no complete message was found or all have been processed
            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
    }
}


ProtocolHandler::ClientState TCPProtocolHandler::process_received(const std::string& message) {
    // Process the message from the server
    // based on the protocol and the Mealy machine logic

    std::string mutableMessage = message;
    size_t pos = message.find(' ');
    std::vector<std::string> parts = messageValidator.split_message(mutableMessage);
    std::string command = parts[0]; //extract first word - command
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    std::string ok_nok;
    if (parts.size() > 1){
        ok_nok = parts[1];
        //to upper
        std::transform(ok_nok.begin(), ok_nok.end(), ok_nok.begin(), ::toupper);
    }



    std::string response;
    FSMValidate::Action action;
    FSMValidate::Action reaction;
    std::string Username = messageValidator.getDisplayName();

    if (command == "MSG"){
        action = FSMValidate::Action::MESSAGE_SERVER;
        reaction = fsm.validate_action(action);
        std::string user;

        if (reaction == FSMValidate::Action::ANY){
            response = messageValidator.validate_message_server(mutableMessage).first.first;
             user = messageValidator.validate_message_server(mutableMessage).first.second;
            if (messageValidator.validate_message_server(mutableMessage).second){
                clientOutput.message_from_server(user, response);
                if(waiting_for_reply){
                    return ProtocolHandler::ClientState::WAITING_FOR_REPLY;
                }
                return ProtocolHandler::ClientState::READY_FOR_INPUT;
            }
            else{
                std::string error = "ERR FROM " + user + " IS Invalid message format\r\n";
                send_message(error);
                send_message("BYE\r\n");
                return ProtocolHandler::ClientState::OVER;
            }
        }
        else if (reaction == FSMValidate::Action::ERROR_USER){
            std::string error = "ERR FROM " + user + " IS You cannot send messages at this state\r\n";
            send_message(error);
            send_message("BYE\r\n");
            return ProtocolHandler::ClientState::OVER;
        }
        return ProtocolHandler::ClientState::READY_FOR_INPUT;
    }

    else if (command == "REPLY"){
        if (ok_nok == "OK") {
            action = FSMValidate::Action::REPLY_SERVER;
            reaction = fsm.validate_action(action);

            if (reaction == FSMValidate::Action::ANY) {
                response = messageValidator.validate_reply(mutableMessage).first;
                if (messageValidator.validate_reply(mutableMessage).second) {
                    if(waiting_for_reply){
                        clientOutput.reply_success(response);
                        waiting_for_reply = false;
                        return ProtocolHandler::ClientState::READY_FOR_INPUT;
                    }
                    else{
                        return ProtocolHandler::ClientState::READY_FOR_INPUT;
                    }
                } else {
                    std::string error = "ERR FROM " + Username + " IS Invalid reply format\r\n";
                    send_message(error);
                    send_message("BYE\r\n");
                    return ProtocolHandler::ClientState::OVER;
                }
            } else if (reaction == FSMValidate::Action::ERROR_USER) {
                std::string error = "ERR FROM " + Username + " IS You cannot send replies at this state\r\n";
                send_message(error);
                send_message("BYE\r\n");
                return ProtocolHandler::ClientState::OVER;
            }

        } else if (ok_nok == "NOK") {
            action = FSMValidate::Action::NREPLY_SERVER;
            reaction = fsm.validate_action(action);

            if (reaction == FSMValidate::Action::ANY) {
                response = messageValidator.validate_reply(mutableMessage).first;
                if (messageValidator.validate_reply(mutableMessage).second) {
                    if(waiting_for_reply){
                        clientOutput.reply_error(response);
                        waiting_for_reply = false;
                        return ProtocolHandler::ClientState::READY_FOR_INPUT;
                    }
                    else{
                        return ProtocolHandler::ClientState::READY_FOR_INPUT;
                    }
                } else {
                    std::string error = "ERR FROM " + Username + " IS Invalid reply format\r\n";
                    send_message(error);
                    send_message("BYE\r\n");
                    return ProtocolHandler::ClientState::OVER;
                }
            } else if (reaction == FSMValidate::Action::ERROR_USER) {
                std::string error = "ERR FROM " + Username + " IS You cannot send replies at this state\r\n";
                send_message(error);
                send_message("BYE\r\n");
                return ProtocolHandler::ClientState::OVER;
            }
        }
    }

    else if (command == "ERR") {
        action = FSMValidate::Action::ERROR_SERVER;
        reaction = fsm.validate_action(action);

        if (reaction == FSMValidate::Action::ANY) {
            response = messageValidator.validate_error_server(mutableMessage).first;
            if (messageValidator.validate_error_server(mutableMessage).second) {
                clientOutput.error_from_server(Username, response);
                send_message("BYE\r\n");
                return ProtocolHandler::ClientState::OVER;
            } else {
                std::string error = "ERR FROM " + Username + " IS Invalid error message\r\n";
                send_message(error);
                send_message("BYE\r\n");
                return ProtocolHandler::ClientState::OVER;
            }
        } else if (reaction == FSMValidate::Action::ERROR_USER) {
            std::string error = "ERR FROM " + Username + " IS You cannot send errors at this state\r\n";
            send_message(error);
            send_message("BYE\r\n");
            return ProtocolHandler::ClientState::OVER;
        }
    } else if (command == "BYE"){
        action = FSMValidate::Action::BYE_SERVER;
        reaction = fsm.validate_action(action);
        if (reaction == FSMValidate::Action::ANY){
            return ProtocolHandler::ClientState::OVER;
        }
        else if (reaction == FSMValidate::Action::ERROR_USER){
            std::string error = "ERR FROM " + Username + " IS You cannot send BYE at this state\r\n";
            send_message(error);
            send_message("BYE\r\n");
            return ProtocolHandler::ClientState::OVER;
        }
    } else {
        std::string error = "ERR FROM " + Username + " IS Invalid command\r\n";
        send_message(error);
        send_message("BYE\r\n");
        return ProtocolHandler::ClientState::OVER;
    }

}
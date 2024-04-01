/**
 * @file TCPProtocolHandler.cpp
 *
 * Implementation of the TCPProtocolHandler class
 * Responsible for handling the TCP protocol in compliance with states of the Mealy machine
 *
 * @Author Marek Effenberger
 */

#include "TCPProtocolHandler.h"


void TCPProtocolHandler::send_message(const std::string& message) {
    // Send the message to the server
    if (send(sockfd, message.c_str(), message.size(), 0) == -1) {
        std::cerr << "Error sending message to the server" << std::endl;
        exit(EXIT_FAILURE);
    }
}


ProtocolHandler::ClientState TCPProtocolHandler::process_user_input(const std::string& message) {

    // My very own command, responsible for termination of the client
    if (message == "/exit"){
        send_message("BYE\r\n");
        return ProtocolHandler::ClientState::OVER;
    }


    // Extracting the first word from the message - the command and making it uppercase
    size_t pos = message.find(' ');
    std::string command = message.substr(0, pos);
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    std::string argument = message.substr(pos + 1);

    // Variables for the response, action and reaction
    std::string response;
    FSMValidate::Action action;
    FSMValidate::Action reaction;

    // Getting the username from the message validator
    std::string Username = messageValidator.getDisplayName();

    //Switch based on the command
    if (command == "/AUTH"){

        action = FSMValidate::Action::AUTHORIZE_USER;
        reaction = fsm.validate_action(action);

        // Any represents that the action is allowed
        if (reaction == FSMValidate::Action::ANY){
            response = messageValidator.authorize_validate(argument).first;
            // Based on the bool we either send it or print it to the user
            if (messageValidator.authorize_validate(argument).second){
                send_message(response);
                waiting_for_reply = true;
                return ProtocolHandler::ClientState::WAITING_FOR_REPLY;
            }
            else{
                // Error occured while validating the message
                clientOutput.internal_error_message(response);
                return ProtocolHandler::ClientState::READY_FOR_INPUT;
            }
        }
        // Warn client represents that the action is not allowed
        else if (reaction == FSMValidate::Action::WARN_CLIENT){
            clientOutput.internal_error_message("You are not authorized to authorize at this state!");
            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
    }
    // Join command
    else if (command == "/JOIN"){
        action = FSMValidate::Action::JOIN_USER;
        reaction = fsm.validate_action(action);

        // Any represents that the action is allowed
        if (reaction == FSMValidate::Action::ANY){
            response = messageValidator.join_validate(argument).first;
            // Based on the bool we either send it or print it to the user
            if (messageValidator.join_validate(argument).second){
                send_message(response);
                waiting_for_reply = true;
                return ProtocolHandler::ClientState::WAITING_FOR_REPLY;
            }
            else{
                // Unsuccesful validation
                clientOutput.internal_error_message(response);
                return ProtocolHandler::ClientState::READY_FOR_INPUT;
            }
        }
        // Warn client represents that the action is not allowed
        else if (reaction == FSMValidate::Action::WARN_CLIENT){
            clientOutput.internal_error_message("You are not authorized to join at this state!");
            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
    }
    // Simple command responsible for renaming
    else if (command == "/RENAME"){
        if (!(messageValidator.rename(argument))){
            clientOutput.internal_error_message("Invalid display name format");
            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
    }
    // Printing the help
    else if (command == "/HELP"){
        clientOutput.print_help();
        return ProtocolHandler::ClientState::READY_FOR_INPUT;
    }
    // Everything else shall be considered message
    else {
        // if first byte / error
        if (message[0] == '/'){
            clientOutput.internal_error_message("Invalid command");
            return ProtocolHandler::ClientState::READY_FOR_INPUT;
        }
        action = FSMValidate::Action::MESSAGE_USER;
        reaction = fsm.validate_action(action);
        // Any represents that the action is allowed
        if (reaction == FSMValidate::Action::ANY){
            response = messageValidator.message_validate(message).first;
            if (messageValidator.message_validate(message).second){
                send_message(response);
            }
            else{
                // Error occured while validating the message
                clientOutput.internal_error_message(response);
            }

        }
        // Warn client represents that the action is not allowed
        else if (reaction == FSMValidate::Action::WARN_CLIENT){
            clientOutput.internal_error_message("You are not authorized to send a message at this state!");
        }
        return ProtocolHandler::ClientState::READY_FOR_INPUT;
    }
}


ProtocolHandler::ClientState TCPProtocolHandler::process_server_message() {
    static std::string buffer; // Static to retain data across calls
    char recvBuffer[1500]; // Buffer with the default size
    ssize_t bytes_received;

    // Iteratively receive the data and glue it together based on the \r\n delimiters
    // If there is a bunch of bytes overexceeding the buffer, they will be processed in the next iteration
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
                buffer.erase(0, pos + 2); // Remove the \r\n from the buffer

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
    std::vector<std::string> parts = messageValidator.split_message(mutableMessage);
    std::string command = parts[0]; // Extract first word - command
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    std::string ok_nok;
    if (parts.size() > 1){
        ok_nok = parts[1];
        //to upper
        std::transform(ok_nok.begin(), ok_nok.end(), ok_nok.begin(), ::toupper);
    }

    // Variables for the response, action and reaction
    std::string response;
    FSMValidate::Action action;
    FSMValidate::Action reaction;
    std::string Username = messageValidator.getDisplayName();

    // Switch based on the command
    // Message from the server
    if (command == "MSG"){
        action = FSMValidate::Action::MESSAGE_SERVER;
        reaction = fsm.validate_action(action);
        std::string user;
        // Any represents that the action is allowed
        if (reaction == FSMValidate::Action::ANY){
            // Extracting the validated data and returning the action based on the validation
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
                // Error occured while validating the message
                std::string error = "ERR FROM " + user + " IS Invalid message format\r\n";
                send_message(error);
                send_message("BYE\r\n");
                return ProtocolHandler::ClientState::OVER;
            }
        }
        // Warn client represents that the action is not allowed in the current state
        else if (reaction == FSMValidate::Action::ERROR_USER){
            std::string error = "ERR FROM " + user + " IS You cannot send messages at this state\r\n";
            send_message(error);
            send_message("BYE\r\n");
            return ProtocolHandler::ClientState::OVER;
        }
        return ProtocolHandler::ClientState::READY_FOR_INPUT;
    }
    // Reply from the server with the logic based on the OK/NOK
    else if (command == "REPLY"){
        if (ok_nok == "OK") {
            action = FSMValidate::Action::REPLY_SERVER;
            reaction = fsm.validate_action(action);
            // Any represents that the action is allowed
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
                // Error occured while validating the message
                } else {
                    std::string error = "ERR FROM " + Username + " IS Invalid reply format\r\n";
                    send_message(error);
                    send_message("BYE\r\n");
                    return ProtocolHandler::ClientState::OVER;
                }
            // Warn client represents that the action is not allowed in the current state
            } else if (reaction == FSMValidate::Action::ERROR_USER) {
                std::string error = "ERR FROM " + Username + " IS You cannot send replies at this state\r\n";
                send_message(error);
                send_message("BYE\r\n");
                return ProtocolHandler::ClientState::OVER;
            }
        // Similiar logic for the NOK
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
    // Error message from the server
    else if (command == "ERR") {
        action = FSMValidate::Action::ERROR_SERVER;
        reaction = fsm.validate_action(action);
        // Any represents that the action is allowed
        if (reaction == FSMValidate::Action::ANY) {
            auto [data, success] = messageValidator.validate_error_server(mutableMessage);
            if (success) {
                // Printing the error message
                clientOutput.error_from_server(data.second, data.first);
                send_message("BYE\r\n");
                return ProtocolHandler::ClientState::OVER;
            } else {
                // Error occured while validating the message
                std::string error = "ERR FROM " + Username + " IS Invalid error message\r\n";
                send_message(error);
                send_message("BYE\r\n");
                return ProtocolHandler::ClientState::OVER;
            }
        // Warn client represents that the action is not allowed in the current state
        } else if (reaction == FSMValidate::Action::ERROR_USER) {
            std::string error = "ERR FROM " + Username + " IS You cannot send errors at this state\r\n";
            send_message(error);
            send_message("BYE\r\n");
            return ProtocolHandler::ClientState::OVER;
        }
    // Bye message from the server
    } else if (command == "BYE"){
        action = FSMValidate::Action::BYE_SERVER;
        reaction = fsm.validate_action(action);
        if (reaction == FSMValidate::Action::ANY){
            printf("Server closed the connection, reaction : %d\n", reaction);
            printf("over %d\n", ProtocolHandler::ClientState::OVER);
            return ProtocolHandler::ClientState::OVER;
        }
        else if (reaction == FSMValidate::Action::ERROR_USER){
            std::string error = "ERR FROM " + Username + " IS You cannot send BYE at this state\r\n";
            send_message(error);
            send_message("BYE\r\n");
            return ProtocolHandler::ClientState::OVER;
        }
    // Unsupported format message
    } else {
        clientOutput.internal_error_message("Invalid command");
        std::string error = messageValidator.form_error_message("Invalid command");
        send_message(error);
        send_message("BYE\r\n");
        return ProtocolHandler::ClientState::OVER;
    }
}

void TCPProtocolHandler::resend_last_message() {
    // Resend the last message to the server
    // Not used in the TCP protocol
    return;
}
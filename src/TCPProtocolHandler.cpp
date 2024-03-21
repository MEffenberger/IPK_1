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


void TCPProtocolHandler::process_user_input(const std::string& message) {
    // Send the message to the server
    // based on the protocol and the Mealy machine logic

    size_t pos = message.find(' ');
    std::string command = message.substr(0, pos);
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);

    std::string argument = message.substr(pos + 1);

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
            }
            else{
                clientOutput.internal_error_message(response);
            }
        }
        else if (reaction == FSMValidate::Action::WARN_CLIENT){
            clientOutput.internal_error_message("You are not authorized to authorize at this state!");
        }
        return;
    }

    else if (command == "/JOIN"){
        action = FSMValidate::Action::JOIN_USER;
        reaction = fsm.validate_action(action);

        if (reaction == FSMValidate::Action::ANY){
            response = messageValidator.join_validate(argument).first;
            //based on the bool we either send it or print it to the user
            if (messageValidator.join_validate(argument).second){
                send_message(response);
            }
            else{
                clientOutput.internal_error_message(response);
            }
        }
        else if (reaction == FSMValidate::Action::WARN_CLIENT){
            clientOutput.internal_error_message("You are not authorized to join at this state!");
        }
        return;
    }

    else if (command == "/RENAME"){

        if (!(messageValidator.rename(argument))){
            clientOutput.internal_error_message("Invalid display name format");
        }
        return;
    }

    else if (command == "/HELP"){
        clientOutput.print_help();
        return;
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
        return;
    }
}


void TCPProtocolHandler::process_server_message() {
    // Process the message from the server
    // based on the protocol and the Mealy machine logic

    FSMValidate::Action action;
    FSMValidate::Action reaction;




}
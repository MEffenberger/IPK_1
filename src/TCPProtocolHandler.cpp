//
// Created by marek on 16.03.2024.
//

#include "TCPProtocolHandler.h"


void TCPProtocolHandler::write_error_message(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}

void TCPProtocolHandler::print_help() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "/auth <id> <secret> <dname> - Authorize the user" << std::endl;
    std::cout << "/join <id> - Join the chat" << std::endl;
    std::cout << "/rename <dname> - Change the display name" << std::endl;
    std::cout << "/help - Print this help message" << std::endl;
    std::cout << "<message> - Send a message to the chat" << std::endl;
}

void TCPProtocolHandler::send_message(const std::string& message) {
    // Send the message to the server
    // based on the protocol and the Mealy machine logic
    if (send(sockfd, message.c_str(), message.size(), 0) == -1) {
        write_error_message("Failed to send the message");
    }
}


void TCPProtocolHandler::process_user_input(const std::string& message) {
    // Send the message to the server
    // based on the protocol and the Mealy machine logic

    size_t pos = message.find(' ');
    std::string command = message.substr(0, pos);
    std::string argument = message.substr(pos + 1);

    std::string response;

    //switch based on the command


}


void TCPProtocolHandler::process_server_message(const std::string& message) {
    return;
}



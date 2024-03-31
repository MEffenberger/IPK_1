/**
 * @file ClientOutput.cpp
 * Implementation of the ClientOutput class
 * This class is responsible for printing messages to the console
 * for the client.
 *
 * @Author Marek Effenberger
 */

#include "ClientOutput.h"

void ClientOutput::message_from_server(const std::string&Username, const std::string& message) {
    std::cout << Username << ": " << message << std::endl;
}

void ClientOutput::internal_error_message(const std::string& message) {
    std::cerr << "ERR: " << message << std::endl;
}

void ClientOutput::print_help() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "/auth <id> <secret> <dname> - Authorize the user" << std::endl;
    std::cout << "/join <id> - Join the chat" << std::endl;
    std::cout << "/rename <dname> - Change the display name" << std::endl;
    std::cout << "/help - Print this help message" << std::endl;
    std::cout << "<message> - Send a message to the chat" << std::endl;
}

void ClientOutput::error_from_server(const std::string&Username, const std::string& message) {
    std::cerr << "ERR FROM " << Username << ": " << message << std::endl;
}

void ClientOutput::reply_success(const std::string& message) {
    std::cerr << "Success: " << message << std::endl;
}

void ClientOutput::reply_error(const std::string& message) {
    std::cerr << "Failure: " << message << std::endl;
}
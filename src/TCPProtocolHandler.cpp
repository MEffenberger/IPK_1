//
// Created by marek on 16.03.2024.
//

#include "TCPProtocolHandler.h"

std::vector<std::string> TCPProtocolHandler::split_message(const std::string& message) {
    std::vector<std::string> result;
    std::string word;
    for (char c : message) {
        if (c == ' ') {
            result.push_back(word);
            word.clear();
        } else {
            word += c;
        }
    }
    result.push_back(word);
    return result;
}

void TCPProtocolHandler::validate_id(const std::string& id) {
    // Validate the id
    // 1*20 (ALPHA / DIGIT / "-")
    if (id.size() < 1 || id.size() > 20) {
        write_error_message("Invalid id length");
        return;
    }
    for (char c : id) {
        if (!isalnum(c) && c != '-') {
            write_error_message("Invalid id character");
            return;
        }
    }
}

void TCPProtocolHandler::validate_secret(const std::string& secret) {
    // Validate the secret
    // 1*128 (ALPHA / DIGIT / "-")
    if (secret.size() < 1 || secret.size() > 128) {
        write_error_message("Invalid secret length");
        return;
    }
    for (char c : secret) {
        if (!isalnum(c) && c != '-') {
            write_error_message("Invalid secret character");
            return;
        }
    }
}

void TCPProtocolHandler::validate_content(const std::string& content) {
    // Validate the content
    // 1*1400 (VCHAR / SP)
    if (content.size() < 1 || content.size() > 1400) {
        write_error_message("Invalid content length");
        return;
    }
    for (char c : content) {
        if (!isprint(c) && c != ' ') {
            write_error_message("Invalid content character");
            return;
        }
    }
}

void TCPProtocolHandler::validate_dname(const std::string& dname) {
    // Validate the dname
    // 1*20 (VCHAR)
    if (dname.size() < 1 || dname.size() > 20) {
        write_error_message("Invalid dname length");
        return;
    }
    for (char c : dname) {
        if (!isprint(c)) {
            write_error_message("Invalid dname character");
            return;
        }
    }
}

void TCPProtocolHandler::write_error_message(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}

std::string TCPProtocolHandler::authorize_validate(const std::string& message) {
    // Validate the message
    std::vector<std::string> parts = split_message(message);
    if (parts.size() != 3){
        write_error_message("Invalid message format");
        return;
    }

    validate_id(parts[1]);
    validate_secret(parts[2]);
    validate_dname(parts[3]);
    std::string to_send = "AUTH" + " " + parts[1] + " " + "AS" + " " + parts[3] + " " + "USING" + " " parts[2] + "\r\n";
    return to_send;

}

std::string TCPProtocolHandler::join_validate(const std::string& message) {
    // Validate the message

    std::vector<std::string> parts = split_message(message);
    if (parts.size() != 1){
        write_error_message("Invalid message format");
        return;
    }

    validate_id(parts[1]);
    std::string to_send = "JOIN" + " " + parts[1] + " " + "AS" + displayName + "\r\n";
    return to_send;

}

std::string TCPProtocolHandler::message_validate(const std::string& message) {
    // Validate the message
    std::vector<std::string> parts = split_message(message);
    if (parts.size() != 1){
        write_error_message("Invalid message format");
        return;
    }

    validate_id(parts[1]);
    validate_content(parts[2]);
    std::string to_send = "MSG FROM" + " " + displayName + " " + "IS" + parts[1] + "\r\n";
    return to_send;

}

std::string TCPProtocolHandler::bye_validate(const std::string& message) {
    // Validate the message
    std::vector <std::string> parts = split_message(message);
    if (parts.size() != 1) {
        write_error_message("Invalid message format");
        return;
    }

    std::string to_send = "BYE" + "\r\n";
    return to_send;
}

std::string TCPProtocolHandler::error_validate(const std::string& message) {
    // Validate the message
    std::vector<std::string> parts = split_message(message);
    if (parts.size() != 1){
        write_error_message("Invalid message format");
        return;
    }

    validate_content(parts[1]);
    std::string to_send = "ERR" + " " + "FROM" + displayName + " " + "IS" + "SOMETHING" + "\r\n"; ////////////
    return to_send;

}

void TCPProtocolHandler::print_help() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "/auth <id> <secret> <dname> - Authorize the user" << std::endl;
    std::cout << "/join <id> - Join the chat" << std::endl;
    std::cout << "/rename <dname> - Change the display name" << std::endl;
    std::cout << "/help - Print this help message" << std::endl;
    std::cout << "<message> - Send a message to the chat" << std::endl;
}

void TCPProtocolHandler::rename(const std::string& dname) {
    // Change the display name
    validate_dname(dname);
    displayName = dname;
}

void TCPProtocolHandler::process_user_input(const std::string& message) {
    // Send the message to the server
    // based on the protocol and the Mealy machine logic

    size_t pos = message.find(' ');
    std::string command = message.substr(0, pos);
    std::string argument = message.substr(pos + 1);

    switch (currentState) {
        case State::START:
            // Send the AUTH message
            // Change the state to AUTH
            if (command == "auth") {
                // Send the message to the server
                // Change the state to AUTH
                authorize_validate_send(argument);
            } else {
                write_error_message("Invalid command");
            }

            break;
        case State::AUTH:
            // Send the OPEN message
            // Change the state to OPEN
            break;
        case State::OPEN:
            // Send the message to the server
            break;
        case State::ERROR:
            // Do nothing
            break;
        case State::END:
            // Do nothing
            break;
    }

}
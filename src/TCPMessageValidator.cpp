//
// Created by marek on 17.03.2024.
//

#include "TCPMessageValidator.h"

std::string TCPMessageValidator::form_error_message(const std::string& message) {
    return "ERR FROM " + displayName + " IS " + message + "\r\n";
}

std::string TCPMessageValidator::form_bye_message() {
    return "BYE\r\n";
}

std::vector<std::string> TCPMessageValidator::split_message(const std::string& message) {
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

bool TCPMessageValidator::validate_id(const std::string& id) {
    // Validate the id
    // 1*20 (ALPHA / DIGIT / "-")
    if (id.size() < 1 || id.size() > 20) {
        return false;
    }
    for (char c : id) {
        if (!isalnum(c) && c != '-') {
            return false;
        }
    }
    return true;
}

bool TCPMessageValidator::validate_secret(const std::string& secret) {
    // Validate the secret
    // 1*128 (ALPHA / DIGIT / "-")
    if (secret.size() < 1 || secret.size() > 128) {
        return false;
    }
    for (char c : secret) {
        if (!isalnum(c) && c != '-') {
            return false;
        }
    }
    return true;
}

bool TCPMessageValidator::validate_content(const std::string& content) {
    // Validate the content
    // 1*1400 (VCHAR / SP)
    if (content.size() < 1 || content.size() > 1400) {
        return false;
    }
    for (char c : content) {
        if (!isprint(c) && c != ' ') {
            return false;
        }
    }
    return true;
}

bool TCPMessageValidator::validate_dname(const std::string& dname) {
    // Validate the dname
    // 1*20 (VCHAR)
    if (dname.size() < 1 || dname.size() > 20) {
        return false;
    }
    for (char c : dname) {
        if (!isprint(c)) {
            return false;
        }
    }
    return true;
}


std::pair<std::string, bool> TCPMessageValidator::authorize_validate(const std::string& message) {
    // Validate the message
    std::vector<std::string> parts = split_message(message);
    if (parts.size() != 3){
        return std::make_pair("Invalid message format", false);
    }

    if (!validate_id(parts[1])) {
        return std::make_pair("Invalid ID format", false);
    }

    if (!validate_secret(parts[2])) {
        return std::make_pair("Invalid secret format", false);
    }

    if (!validate_dname(parts[3])) {
        return std::make_pair("Invalid display name format", false);
    }
    displayName = parts[3];

    std::string to_send = "AUTH " + parts[1] + " AS " + parts[3] + " USING " + parts[2] + "\r\n";;
    return std::make_pair(to_send, true);

}


std::pair<std::string, bool> TCPMessageValidator::join_validate(const std::string& message) {
    // Validate the message
    std::vector<std::string> parts = split_message(message);
    if (parts.size() != 1){
        return std::make_pair("Invalid message format", false);
    }

    if (!validate_id(parts[1])) {
        return std::make_pair("Invalid ID format", false);
    }

    std::string to_send = "JOIN " + parts[1] + " AS " + displayName + "\r\n";
    return std::make_pair(to_send, true);

}


std::pair<std::string, bool> TCPMessageValidator::message_validate(const std::string& message) {
    // Validate the message
//    std::vector<std::string> parts = split_message(message);
//    if (parts.size() != 1){
//        return std::make_pair("Invalid message format", false);
//    }

//    if (!validate_content(parts[1])) {
//        return std::make_pair("Invalid content format", false);
//    }
    if (!validate_content(message)) {
        return std::make_pair("Invalid content format", false);
    }

    std::string to_send = "MSG FROM " + displayName + " IS " + message + "\r\n";
    return std::make_pair(to_send, true);
}

bool TCPMessageValidator::rename(const std::string& dname) {
    // Change the display name
    if (!validate_dname(dname)){
        return false;
    }
    displayName = dname;
    return true;
}

std::pair<bool, std::string> TCPMessageValidator::validate_reply(const std::string& message) {
    // Validate the message
    std::vector<std::string> parts = split_message(message);

    // REPLY OK/NOK IS <content>
    // content can have spaces so cannot validate the size
    if (parts[0] != "REPLY" || (parts[1] != "OK" && parts[1] != "NOK") || parts[2] != "IS") {
        return false;
    }
    // now glue the content back together and validate it, add whitespace
    std::string content;
    for (int i = 3; i < parts.size(); i++) {
        content += parts[i] + " ";
    }
    content.pop_back();
    return std::make_pair(validate_content(content), parts[1]);
}

bool TCPMessageValidator::validate_message_server(const std::string& message) {
    // Validate the message
    std::vector<std::string> parts = split_message(message);

    if (parts[0] != "MSG" || parts[1] != "FROM" || parts[3] != "IS") {
        return false;
    }

    if (!validate_dname(parts[2])) {
        return false;
    }
    // now glue the content back together and validate it, add whitespace
    std::string content;
    for (int i = 4; i < parts.size(); i++) {
        content += parts[i] + " ";
    }
    content.pop_back();
    return validate_content(content);
}

bool TCPMessageValidator::validate_error_server(const std::string& message) {
    // Validate the message
    std::vector<std::string> parts = split_message(message);

    if (parts[0] != "ERR" || parts[1] != "FROM" || parts[3] != "IS") {
        return false;
    }

    if (!validate_dname(parts[2])) {
        return false;
    }
    // now glue the content back together and validate it, add whitespace
    std::string content;
    for (int i = 4; i < parts.size(); i++) {
        content += parts[i] + " ";
    }
    content.pop_back();
    return validate_content(content);
}

bool TCPMessageValidator::validate_bye_server(const std::string& message) {
    // Validate the message
    std::vector<std::string> parts = split_message(message);
    if (parts.size() != 1) {
        return false;
    }
    if (message != "BYE") {
        return false;
    }
    return true;
}

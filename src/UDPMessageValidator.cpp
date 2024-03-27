//
// Created by marek on 27.03.2024.
//

#include "UDPMessageValidator.h"

std::vector<std::string> UDPMessageValidator::split_message(const std::string& message) {

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

bool UDPMessageValidator::validate_id(const std::string& id) {
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

bool UDPMessageValidator::validate_secret(const std::string& secret) {
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

bool UDPMessageValidator::validate_content(const std::string& content) {
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

bool UDPMessageValidator::validate_dname(const std::string& daname) {
    // Validate the dname
    // 1*20 (VCHAR)

    if (daname.size() < 1 || daname.size() > 20) {
        return false;
    }
    for (char c : daname) {
        if (!isprint(c)) {
            return false;
        }
    }
    return true;
}

std::pair<std::string, bool> UDPMessageValidator::authorize_validate(const std::string& message) {
    // Validate the message

    std::vector<std::string> parts = split_message(message);
    if (parts.size() != 3){
        return std::make_pair("Invalid message format", false);
    }

    if (!validate_id(parts[0])) {
        return std::make_pair("Invalid ID format", false);
    }

    if (!validate_secret(parts[1])) {
        return std::make_pair("Invalid secret format", false);
    }



    if (!validate_dname(parts[2])) {
        return std::make_pair("Invalid display name format", false);
    }
    displayName = parts[2];

    std::string to_send = "AUTH " + parts[0] + " AS " + parts[2] + " USING " + parts[1] + "\r\n";;
    return std::make_pair(to_send, true);

}


std::pair<std::string, bool> UDPMessageValidator::join_validate(const std::string& message) {
    // Validate the message
    std::vector<std::string> parts = split_message(message);
    if (parts.size() != 1){
        return std::make_pair("Invalid message format", false);
    }

    if (!validate_id(parts[0])) {
        return std::make_pair("Invalid ID format", false);
    }

    std::string to_send = "JOIN " + parts[0] + " AS " + displayName + "\r\n";
    return std::make_pair(to_send, true);

}


std::pair<std::string, bool> UDPMessageValidator::message_validate(const std::string& message) {

    if (!validate_content(message)) {
        return std::make_pair("Invalid content format", false);
    }

    std::string to_send = "MSG FROM " + displayName + " IS " + message + "\r\n";
    return std::make_pair(to_send, true);
}

bool UDPMessageValidator::rename(const std::string& daname) {
    // Change the display name
    if (!validate_dname(daname)){
        return false;
    }
    displayName = daname;
    return true;
}

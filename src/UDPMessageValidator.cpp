//
// Created by marek on 27.03.2024.
//

#include "UDPMessageValidator.h"

std::vector<uint8_t> UDPMessageValidator::form_bye_message(uint16_t messageID) {
    std::vector<uint8_t> response;
    response.push_back(0xFF);

    // Add the messageID (2 bytes, big endian)
    response.push_back(static_cast<uint8_t>((messageID >> 8) & 0xFF));
    response.push_back(static_cast<uint8_t>(messageID & 0xFF));

    return response;
}

std::vector<uint8_t > UDPMessageValidator::form_error_message(uint16_t messageID, const std::string& dname,
                                                              const std::string& error){
    std::vector<uint8_t> response;
    response.push_back(0xFE);
    response.push_back(static_cast<uint8_t>((messageID >> 8) & 0xFF));
    response.push_back(static_cast<uint8_t>(messageID & 0xFF));

    response.insert(response.end(), dname.begin(), dname.end());
    response.push_back('\0');

    response.insert(response.end(), error.begin(), error.end());
    response.push_back('\0');

    return response;
}

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

std::string UDPMessageValidator::get_display_name() const {
    return displayName;
}

bool UDPMessageValidator::rename(const std::string& name) {
    if (validate_dname(name)) {
        displayName = name;
        return true;
    }
    return false;
}

std::pair<std::vector<uint8_t>, bool> UDPMessageValidator::authorize_validate(int8_t code, uint16_t messageID,
                                                                              const std::string& username,
                                                                              const std::string& deename,
                                                                              const std::string& secret) {

    std::vector<uint8_t> response;

    if (!validate_id(username) || !validate_dname(deename) || !validate_secret(secret)) {
        std::string err = "ERROR Invalid username, display name or secret";
        response.insert(response.end(), err.begin(), err.end());
        return std::make_pair(response, false);
    }
    displayName = deename;
    // Add the code (1 byte)
    response.push_back(code);

    // Add the messageID (2 bytes, big endian)
    response.push_back(static_cast<uint8_t>((messageID >> 8)));
    response.push_back(static_cast<uint8_t>(messageID & 0xFF));

    // Add the username and null terminator
    response.insert(response.end(), username.begin(), username.end());
    response.push_back('\0');

    // Add the displayName and null terminator
    response.insert(response.end(), displayName.begin(), displayName.end());
    response.push_back('\0');

    // Add the secret and null terminator
    response.insert(response.end(), secret.begin(), secret.end());
    response.push_back('\0');

    return std::make_pair(response, true);
}


std::pair<std::vector<uint8_t>, bool> UDPMessageValidator::join_validate(int8_t code, uint16_t messageID,
                                                                         const std::string& channel) {

    std::vector<uint8_t> response;

    if (!validate_id(channel)) {
        std::string err = "ERROR Invalid channel or display name while joining";
        response.insert(response.end(), err.begin(), err.end());
        return std::make_pair(response, false);
    }
    // Add the code (1 byte)
    response.push_back(code);

    // Add the messageID (2 bytes, big endian)
    response.push_back(static_cast<uint8_t>((messageID >> 8)));
    response.push_back(static_cast<uint8_t>(messageID & 0xFF));

    // Add the channel and null terminator
    response.insert(response.end(), channel.begin(), channel.end());
    response.push_back('\0');

    // Add the displayName and null terminator
    response.insert(response.end(), displayName.begin(), displayName.end());
    response.push_back('\0');

    return std::make_pair(response, true);
}

std::pair<std::vector<uint8_t>, bool> UDPMessageValidator::message_validate(int8_t code, uint16_t messageID,
                                                                            const std::string& content) {

    std::vector<uint8_t> response;

    if (!validate_content(content)) {
        std::string err = "ERROR Invalid content";
        response.insert(response.end(), err.begin(), err.end());
        return std::make_pair(response, false);
    }
    // Add the code (1 byte)
    response.push_back(code);

    // Add the messageID (2 bytes, big endian)
    response.push_back(static_cast<uint8_t>((messageID >> 8)));
    response.push_back(static_cast<uint8_t>(messageID & 0xFF));

    // Add the displayName and null terminator
    response.insert(response.end(), displayName.begin(), displayName.end());
    response.push_back('\0');

    // Add the content and null terminator
    response.insert(response.end(), content.begin(), content.end());
    response.push_back('\0');

    return std::make_pair(response, true);
}

std::pair<std::vector<std::string>, bool> UDPMessageValidator::parse_and_validate(const std::vector<uint8_t>& message, std::string type) {

    std::vector<std::string> result;
    std::string word;

    for (char c : message) {
        if (c == '\0') {
            result.push_back(word);
            word.clear();
        } else {
            word += c;
        }
    }

    std::string invalid = "Invalid Arguments Provided";

    if (type == "reply"){
        if (result.size() != 1){
            result.clear();
            result.push_back(invalid);
            return std::make_pair(result, false);
        } else if (!validate_content(result[0])){
            result.clear();
            result.push_back(invalid);
            return std::make_pair(result, false);
        } else {
            return std::make_pair(result, true);
        }
    } else if (type == "msg"){
        if (result.size() != 2){
            result.clear();
            result.push_back(invalid);
            return std::make_pair(result, false);
        } else if (!validate_dname(result[0]) || !validate_content(result[1])){
            result.clear();
            result.push_back(invalid);
            return std::make_pair(result, false);
        } else {
            return std::make_pair(result, true);
        }
    } else if (type == "err"){
        if (result.size() != 2){
            result.clear();
            result.push_back(invalid);
            return std::make_pair(result, false);
        } else if (!validate_dname(result[0]) || !validate_content(result[1])){
            result.clear();
            result.push_back(invalid);
            return std::make_pair(result, false);
        } else {
            return std::make_pair(result, true);
        }
    } else {
        result.clear();
        result.push_back(invalid);
        return std::make_pair(result, false);
    }
}
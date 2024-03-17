//
// Created by marek on 17.03.2024.
//

#ifndef IPK_1_TCPMESSAGEVALIDATOR_H
#define IPK_1_TCPMESSAGEVALIDATOR_H

#include <string>
#include <vector>

class TCPMessageValidator {

public:

    TCPMessageValidator() = default;


    std::pair<std::string, bool> authorize_validate(const std::string& message);
    std::pair<std::string, bool> join_validate(const std::string& message);
    std::pair<std::string, bool> message_validate(const std::string& message);
    std::pair<std::string, bool> bye_validate(const std::string& message);
    std::pair<std::string, bool> error_validate(const std::string& message);


    std::pair<std::string, bool> reply_validate(const std::string& message);
    std::pair<std::string, bool> nreply_validate(const std::string& message);
    bool validate_dname(const std::string& dname);
    bool rename(const std::string& dname);
    std::string form_error_message(const std::string& message);
    std::string form_bye_message();

    std::pair<bool, std::string> validate_reply(const std::string& message);
    bool validate_message_server(const std::string& message);
    bool validate_error_server(const std::string& message);
    bool validate_bye_server(const std::string& message);

private:

    std::string displayName;

    std::vector<std::string> split_message(const std::string& message);
    bool validate_id(const std::string& id);
    bool validate_secret(const std::string& secret);
    bool validate_content(const std::string& content);

};


#endif //IPK_1_TCPMESSAGEVALIDATOR_H

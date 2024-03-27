//
// Created by marek on 27.03.2024.
//

#ifndef VUT_IPK_UDPMESSAGEVALIDATOR_H
#define VUT_IPK_UDPMESSAGEVALIDATOR_H

#include <string>
#include <vector>


class UDPMessageValidator {


public:
    std::string displayName;

    bool rename(const std::string& name);


private:
    bool validate_id(const std::string& id);
    bool validate_secret(const std::string& secret);
    bool validate_content(const std::string& content);
    bool validate_dname(const std::string& daname);
    std::vector<std::string> split_message(const std::string& message);
};


#endif //VUT_IPK_UDPMESSAGEVALIDATOR_H

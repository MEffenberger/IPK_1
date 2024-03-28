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
    std::string get_display_name() const;

    std::vector<std::string> split_message(const std::string& message);

    std::pair<std::vector<uint8_t>, bool> authorize_validate(int8_t code, uint16_t messageID,
                                                                                  const std::string& username,
                                                                                  const std::string& deename,
                                                                                  const std::string& secret);

    std::pair<std::vector<uint8_t>, bool> join_validate(int8_t code, uint16_t messageID,
                                                             const std::string& channel);

    std::pair<std::vector<uint8_t>, bool> message_validate(int8_t code, uint16_t messageID,
                                                                const std::string& content);

    std::vector<uint8_t> form_bye_message(uint16_t messageID);


private:
    bool validate_id(const std::string& id);
    bool validate_secret(const std::string& secret);
    bool validate_content(const std::string& content);
    bool validate_dname(const std::string& daname);
};


#endif //VUT_IPK_UDPMESSAGEVALIDATOR_H

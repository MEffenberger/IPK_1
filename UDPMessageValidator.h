/**
 * @file UDPMessageValidator.h
 *
 * Declaration of the UDPMessageValidator class
 *
 * @Author Marek Effenberger
 */

#ifndef VUT_IPK_UDPMESSAGEVALIDATOR_H
#define VUT_IPK_UDPMESSAGEVALIDATOR_H

#include <string>
#include <vector>

/**
 * Class responsible for generation and validation of the incoming messages
 * in the udp protocol
 * And also for the creation of the outgoing messages
 */
class UDPMessageValidator {


public:
    std::string displayName;

    /**
     * Rename the display name
     * @param name
     * @return Success of the operation
     */
    bool rename(const std::string& name);

    /**
     * Get the display name
     * @return Display name
     */
    std::string get_display_name() const;

    /**
     * Split the message by spaces
     * @param message
     * @return Vector of the words
     */
    std::vector<std::string> split_message(const std::string& message);

    /**
     * Validate the authorize message
     * @param code
     * @param messageID
     * @param username
     * @param deename
     * @param secret
     * @return pair of the message and the result of the validation
     */
    std::pair<std::vector<uint8_t>, bool> authorize_validate(int8_t code, uint16_t messageID,
                                                                                  const std::string& username,
                                                                                  const std::string& deename,
                                                                                  const std::string& secret);

    /**
     * Validate the join message
     * @param code
     * @param messageID
     * @param channel
     * @return pair of the message and the result of the validation
     */
    std::pair<std::vector<uint8_t>, bool> join_validate(int8_t code, uint16_t messageID,
                                                             const std::string& channel);

    /**
     * Validate the message
     * @param code
     * @param messageID
     * @param content
     * @return pair of the message and the result of the validation
     */
    std::pair<std::vector<uint8_t>, bool> message_validate(int8_t code, uint16_t messageID,
                                                                const std::string& content);

    /**
     * Form bye message
     * @param code
     * @param messageID
     * @return Bye message
     */
    std::vector<uint8_t> form_bye_message(uint16_t messageID);

    /**
     * Validate the message from the server
     * @param message
     * @return pair of the message, dname and the result of the validation
     */
    std::pair<std::vector<std::string>, bool> parse_and_validate(const std::vector<uint8_t>& message, std::string type);

    /**
     * Form the error message
     * @param messageID
     * @param dname
     * @param error
     * @return Error message
     */
    std::vector<uint8_t> form_error_message(uint16_t messageID, const std::string& dname,const std::string& error);



private:

    /**
     * Validate the id
     * @param id
     * @return success of the validation
     */
    bool validate_id(const std::string& id);

    /**
     * Validate the secret
     * @param secret
     * @return success of the validation
     */
    bool validate_secret(const std::string& secret);

    /**
     * Validate the content
     * @param content
     * @return success of the validation
     */
    bool validate_content(const std::string& content);

    /**
     * Validate the display name
     * @param daname
     * @return success of the validation
     */
    bool validate_dname(const std::string& daname);
};


#endif //VUT_IPK_UDPMESSAGEVALIDATOR_H

/**
 * @file TCPMessageValidator.h
 *
 * Class responsible for validation of the messages in the tcp protocol
 *
 * @Author Marek Effenberger
 */

#ifndef IPK_1_TCPMESSAGEVALIDATOR_H
#define IPK_1_TCPMESSAGEVALIDATOR_H

#include <string>
#include <vector>
#include <algorithm>

/**
 * Class responsible for the validation of the messages in the tcp protocol
 */
class TCPMessageValidator {

public:
    // Constructor and destructor
    TCPMessageValidator() = default;
    ~TCPMessageValidator() = default;
    // Display name of the client
    std::string displayName;

    /**
     * Validate the authorize message
     * @param message
     * @return pair of the message and the result of the validation
     */
    std::pair<std::string, bool> authorize_validate(const std::string& message);

    /**
     * Validate the join message
     * @param message
     * @return pair of the message and the result of the validation
     */
    std::pair<std::string, bool> join_validate(const std::string& message);

    /**
     * Validate the message
     * @param message
     * @return pair of the message and the result of the validation
     */
    std::pair<std::string, bool> message_validate(const std::string& message);

    /**
     * Validate the bye message
     * @param message
     * @return pair of the message and the result of the validation
     */
    std::pair<std::string, bool> bye_validate(const std::string& message);

    /**
     * Validate the error message
     * @param message
     * @return pair of the message and the result of the validation
     */
    std::pair<std::string, bool> error_validate(const std::string& message);

    /**
     * Validate the display name
     * @param message
     * @return success of the validation
     */
    bool validate_dname(const std::string& daname);

    /**
     * Rename the display name
     * @param daname
     * @return success of the renaming
     */
    bool rename(const std::string& daname);

    /**
     * Form the error message
     * @param message
     * @return the error message
     */
    std::string form_error_message(const std::string& message);

    /**
     * Form the bye message
     * @param message
     * @return the message
     */
    std::string form_bye_message();

    /**
     * Validate the reply message
     * @param message
     * @return pair of the message and the result of the validation
     */
    std::pair<std::string, bool> validate_reply(const std::string& message);

    /**
     * Validate the message from the server
     * @param message
     * @return pair of the message. dname and the result of the validation
     */
    std::pair<std::pair<std::string, std::string>, bool> validate_message_server(const std::string& message);

    /**
     * Validate the error message from the server
     * @param message
     * @return pair of the message, dname and the result of the validation
     */
    std::pair<std::pair<std::string, std::string>, bool> validate_error_server(const std::string& message);

    /**
     * Validate the bye message from the server
     * @param message
     * @return success of the validation
     */
    bool validate_bye_server(const std::string& message);

    /**
     * Get the display name
     * @return display name
     */
    std::string getDisplayName() const;

    /**
     * Split the message
     * @param message
     * @return vector of the parts of the message
     */
    std::vector<std::string> split_message(const std::string& message);

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

};


#endif //IPK_1_TCPMESSAGEVALIDATOR_H

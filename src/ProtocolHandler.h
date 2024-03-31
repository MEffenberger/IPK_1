/**
 * @file ProtocolHandler.h
 *
 * This file contains the declaration of the ProtocolHandler interface
 *
 * @Author Marek Effenberger
 */

#ifndef IPK_1_PROTOCOLHANDLER_H
#define IPK_1_PROTOCOLHANDLER_H

#include <string>

/**
 * ProtocolHandler interface
 */
class ProtocolHandler {
public:
    /**
     * Client state, needed for general handling of the client
     */
    enum class ClientState {
        WAITING_FOR_CONFIRMATION,
        WAITING_FOR_REPLY,
        READY_FOR_INPUT,
        OVER
    };

    /**
     * Process the message received from the server
     * @return client state
     */
    virtual ClientState process_server_message() = 0;

    /**
     * Process the message received from the user
     * @param message a line of text from the user
     * @return client state
     */
    virtual ClientState process_user_input(const std::string& message) = 0;

    /**
     * Resend the last message
     */
    virtual void resend_last_message() = 0;

    /**
     * Destructor
     */
    virtual ~ProtocolHandler() = default;
};


#endif //IPK_1_PROTOCOLHANDLER_H

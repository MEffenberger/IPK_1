/**
 * @file UDPProtocolHandler.h
 *
 * This file contains the declaration of the UDPProtocolHandler class
 *
 * @Author Marek Effenberger
 */

#ifndef VUT_IPK_UDPPROTOCOLHANDLER_H
#define VUT_IPK_UDPPROTOCOLHANDLER_H

#include "ProtocolHandler.h"
#include "FSMValidate.h"
#include "ClientOutput.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <cstring>
#include "UDPMessageValidator.h"
#include <algorithm>

/**
 * UDP Protocol Handler
 * Class responsible for the conforming to the UDP protocol
 * utilizing the FSMValidate and ClientOutput classes
 */
class UDPProtocolHandler : public ProtocolHandler {
private:
    int sockfd;
    // Class attributes
    FSMValidate fsm;
    ClientOutput clientOutput;
    struct sockaddr_in server_address;
    // Vector of received message IDs, to prevent duplicates
    std::vector<uint16_t> received_ids;
    // ID just sent
    uint16_t awaited_conirm_id;
    // ID awaited
    uint16_t awaited_reply_id;
    // Nevereending incrementing ID
    uint16_t renamer3000;
    // Flags
    bool waiting_for_reply = false;
    // Used for resending
    std::vector<uint8_t> last_message;
    // Flags for error handling
    bool error_sent = false;
    bool awaiting_over = false;
    bool waiting_for_confirmation = false;
    bool error_while_waiting = false;
    // Error message for sending if error encountered while waiting for confirmation
    std::string error_message;
    // Message validator
    UDPMessageValidator messageValidator;

    /**
     * Send the message to the server
     * @param message
     */
    void send_message(const std::vector<uint8_t>& message);

    /**
     * Send the confirmation message
     * @param messageID
     */
    void send_confirmation(uint16_t messageID);

    /**
     * Process the received message, validate it and return the client state
     * @param message
     * @return client state
     */
    ProtocolHandler::ClientState process_received(const std::vector<uint8_t>& message);

    /**
     * Check if the ID is already in the vector
     * @param id
     * @return success of the lookup
     */
    bool id_lookup(uint16_t id);

    /**
     * Send the error message
     * @param error
     */
    void send_error_message(const std::string& error);

public:
    // Constructor
    UDPProtocolHandler(int fd, struct sockaddr_in serverAddr)
            : sockfd(fd),
              fsm(),
              server_address(serverAddr),
              awaited_conirm_id(0),
              awaited_reply_id(0),
              renamer3000(0),
              waiting_for_reply(false),
              error_sent(false),
              awaiting_over(false),
              waiting_for_confirmation(false),
              error_while_waiting(false)
    {
        // Initialize vector with default values
        received_ids = std::vector<uint16_t>();
        last_message = std::vector<uint8_t>();
        error_message = std::string();
    }

    /**
     * Resend the last message
     */
    void resend_last_message() override;

    /**
     * Process the message received from the server
     * @return client state
     */
    ProtocolHandler::ClientState process_server_message() override;

    /**
     * Process the message received from the user
     * @param message
     * @return client state
     */
    ProtocolHandler::ClientState process_user_input(const std::string& message) override;
};


#endif //VUT_IPK_UDPPROTOCOLHANDLER_H

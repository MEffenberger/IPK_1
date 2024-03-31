/**
 * @file TCPProtocolHandler.h
 *
 * Declarations of the TCPProtocolHandler class
 * Responsible for handling the TCP protocol in compliance with states of the Mealy machine
 *
 * @Author Marek Effenberger
 */

#ifndef IPK_1_TCPPROTOCOLHANDLER_H
#define IPK_1_TCPPROTOCOLHANDLER_H

#include "ProtocolHandler.h"
#include "TCPMessageValidator.h"
#include <iostream>
#include <ostream>
#include <sys/socket.h>
#include "FSMValidate.h"
#include "ClientOutput.h"
#include <algorithm>
#include <unistd.h>
#include <cstring>

/**
 * TCPProtocolHandler class
 * Responsible for handling the TCP protocol in compliance with states of the Mealy machine
 */
class TCPProtocolHandler : public ProtocolHandler {
private:

    // Class attributes
    TCPMessageValidator messageValidator;
    int sockfd;
    FSMValidate fsm;
    bool waiting_for_reply = false;
    ClientOutput clientOutput;

    /**
     * Send the message to the server
     * @param message
     */
    void send_message(const std::string& message);

    /**
     * Process the received message, validate it and return the client state
     * @param message
     * @return client state
     */
    ProtocolHandler::ClientState process_received(const std::string& message);


public:
    // Constructor
    explicit TCPProtocolHandler(int fd) : messageValidator(), sockfd(fd), fsm() {}

    /**
     * Resend the last message, not implemented
     */
    void resend_last_message() override;

    /**
     * Process the message received from the server, recv on while loop
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


#endif //IPK_1_TCPPROTOCOLHANDLER_H

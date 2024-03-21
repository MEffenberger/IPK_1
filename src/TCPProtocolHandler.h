//
// Created by marek on 16.03.2024.
//

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


class TCPProtocolHandler : public ProtocolHandler {
private:

    // constructor

    TCPMessageValidator messageValidator;
    int sockfd; // Socket file descriptor
    FSMValidate fsm;
    bool waiting_for_reply = false;
    ClientOutput clientOutput;


    void send_message(const std::string& message);



public:
    TCPProtocolHandler(int fd) : messageValidator(), sockfd(fd), fsm() {}


    void process_server_message() override;

    void process_user_input(const std::string& message) override;
};


#endif //IPK_1_TCPPROTOCOLHANDLER_H

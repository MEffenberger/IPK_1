//
// Created by marek on 14.03.2024.
//

#ifndef IPK_1_TCP_LOGIC_H
#define IPK_1_TCP_LOGIC_H

#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <memory>
#include <sys/socket.h>
#include <string>
#include <cstring>

#include "ProtocolHandler.h"
#include "TCPProtocolHandler.h"
//#include "UDPProtocolHandler.h"


class StreamHandler {
private:
    int sockfd; // Socket file descriptor
    int epoll_fd; // Epoll file descriptor
    std::string protocol; // The protocol used for communication
    std::unique_ptr<ProtocolHandler> protocolHandler;
    // ProtocolStateMachine protocol_fsm; // The protocol state machine

public:
    StreamHandler(int socket_fd, std::string protocol_type) : sockfd(socket_fd), epoll_fd(-1), protocol(protocol_type) {
        if (protocol == "tcp") {
            protocolHandler = std::make_unique<TCPProtocolHandler>(sockfd);
//        } else if (protocol == "udp") {
//            protocolHandler = std::make_unique<UDPProtocolHandler>();
        } else {
            // Cannot get here really
            std::cerr << "Invalid protocol provided " << protocol << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void run_event_loop();

private:
    void add_to_epoll(int fd, uint32_t events);
};

#endif //IPK_1_TCP_LOGIC_H

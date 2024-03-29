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
#include <fcntl.h>
#include <queue>

#include "ProtocolHandler.h"
#include "TCPProtocolHandler.h"
#include "UDPProtocolHandler.h"
#include "interruptSignal.h"


class StreamHandler {
private:
    int sockfd; // Socket file descriptor
    int epoll_fd; // Epoll file descriptor
    std::string protocol; // The protocol used for communication
    uint8_t retry_count;
    uint16_t confirmation_timeout;
    std::unique_ptr<ProtocolHandler> protocolHandler;
    struct sockaddr_in server_address;



    // ProtocolStateMachine protocol_fsm; // The protocol state machine

public:
    StreamHandler(int socket_fd, std::string protocol_type, uint8_t retry_count, uint16_t confirmation_timeout, struct sockaddr_in server_address) : sockfd(socket_fd), epoll_fd(-1), protocol(protocol_type), retry_count(retry_count), confirmation_timeout(confirmation_timeout),
                                                                                                                                                     server_address(server_address) {
        if (protocol == "tcp") {
            protocolHandler = std::make_unique<TCPProtocolHandler>(sockfd);
        } else if (protocol == "udp") {
            protocolHandler = std::make_unique<UDPProtocolHandler>(sockfd, retry_count, confirmation_timeout, server_address);
        }
    }


    void run_event_loop();

private:
};

#endif //IPK_1_TCP_LOGIC_H

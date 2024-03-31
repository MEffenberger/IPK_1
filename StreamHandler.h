/**
 * @file StreamHandler.h
 * Declarations of the StreamHandler class.
 * Main purpose of this class is to run the event loop and communicate with the interface
 *
 * @Author Marek Effenberger
 */

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

/**
 * StreamHandler class
 * Main purpose of this class is to run the event loop and communicate with the interface
 */
class StreamHandler {
private:

    int sockfd;
    std::string protocol;
    uint8_t retry_count;
    uint16_t confirmation_timeout;
    std::unique_ptr<ProtocolHandler> protocolHandler;
    struct sockaddr_in server_address;

public:
    /**
     * Constructor
     * The Specific handler is chosen based on the protocol type
     * @param socket_fd
     * @param protocol_type
     * @param retry_count
     * @param confirmation_timeout
     * @param server_address
     */
    StreamHandler(int socket_fd, std::string protocol_type, uint8_t retry_count, uint16_t confirmation_timeout, struct sockaddr_in server_address) : sockfd(socket_fd), protocol(protocol_type), retry_count(retry_count), confirmation_timeout(confirmation_timeout),
                                                                                                                                                     server_address(server_address) {
        if (protocol == "tcp") {
            protocolHandler = std::make_unique<TCPProtocolHandler>(sockfd);
        } else if (protocol == "udp") {
            protocolHandler = std::make_unique<UDPProtocolHandler>(sockfd, server_address);
        }
    }

    /**
     * Run the event loop
     */
    void run_event_loop();

private:
};

#endif //IPK_1_TCP_LOGIC_H

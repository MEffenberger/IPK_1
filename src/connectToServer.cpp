/**
 * @file connectToServer.cpp
 * Implementation of connectToServer function
 * Connects to the server using the provided configuration
 * Returns the socket file descriptor
 *
 * @Author Marek Effenberger
 */

#include "connectToServer.h"


int connectToServer(const Config& config) {
    // Create hints and servinfo structures, helper pointers
    struct addrinfo hints{}, *servinfo, *p;
    // default to -1
    int sockfd = -1;
    int status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4 only
    hints.ai_socktype = (config.protocol == "tcp") ? SOCK_STREAM : SOCK_DGRAM; // TCP or UDP

    // Get address info
    status = getaddrinfo(config.server_ip, std::to_string(config.server_port).c_str(), &hints, &servinfo);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return -1;
    }

    // Go through the linkedlist of addrinfo structures, connect to the first one that works
    for (p = servinfo; p != NULL; p = p->ai_next) {


        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (config.protocol == "udp") {
            freeaddrinfo(servinfo);
            return sockfd;
        }

        if (sockfd != -1) {
            if (::connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                std::cerr << "connect error: " << strerror(errno) << std::endl;
                close(sockfd);
            } else {
                freeaddrinfo(servinfo);
                return sockfd;
            }

        } else {
            std::cerr << "socket error: " << strerror(errno) << std::endl;
        }
    }

    std::cerr << "Unable to connect to the server" << std::endl;
    freeaddrinfo(servinfo);
    return -1; // Return error status
}

// Needed for UDP
struct sockaddr_in getServerAddress(const Config& config) {
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(config.server_port);
    inet_pton(AF_INET, config.server_ip, &server_address.sin_addr);
    return server_address;
}
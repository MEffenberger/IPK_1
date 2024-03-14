//
// Created by marek on 14.03.2024.
//

#include "connect.h"

int connect(const Config& config) {

    // Gonna need hints to specify what kind of address we want to get and servinfo to store the results with p being the pointer to the current result
    struct addrinfo hints{}, *servinfo, *p;
    // Setting sockfd to -1 to indicate an error
    int sockfd = -1;
    // Status of the getaddrinfo function
    int status;

    memset(&hints, 0, sizeof(hints));
    // IPv4 only
    hints.ai_family = AF_INET;
    // TCP or UDP based on the config
    hints.ai_socktype = SOCK_STREAM ? config.protocol == "tcp" : SOCK_DGRAM;

    // Get address information for the host
    status = getaddrinfo(config.server_ip, std::to_string(config.server_port).c_str(), &hints, &servinfo);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return -1;
    }

    // servinfo now points to a linked list of struct addrinfos
    // Attempt to connect to the first one I can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        // Create a new socket file descriptor
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        // If the socket file descriptor is valid, attempt to connect to the server
        if (sockfd != -1 && connect(sockfd, p->ai_addr, p->ai_addrlen) != -1) {
            freeaddrinfo(servinfo);
            // LETS GO
            return sockfd;
        }
        close(sockfd);
    }

    // If we got here, it means we were unable to connect to the server
    std::cerr << "Unable to connect to the server" << std::endl;
    freeaddrinfo(servinfo); // all done with this structure
    return sockfd; // Return the -1 (error) status
}
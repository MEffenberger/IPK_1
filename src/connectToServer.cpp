//
// Created by marek on 14.03.2024.
//

#include "connectToServer.h"

int connectToServer(const Config& config) {
    struct addrinfo hints{}, *servinfo, *p;
    int sockfd = -1;
    int status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4 only
    hints.ai_socktype = (config.protocol == "tcp") ? SOCK_STREAM : SOCK_DGRAM;

    status = getaddrinfo(config.server_ip, std::to_string(config.server_port).c_str(), &hints, &servinfo);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return -1;
    }
    printf("Connecting to the server\n");
    for (p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd != -1) {
            if (::connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                std::cerr << "connect error: " << strerror(errno) << std::endl;
                close(sockfd);
            } else {
                freeaddrinfo(servinfo);
                printf("Connected to the server\n");
                return sockfd;
            }
        } else {
            std::cerr << "socket error: " << strerror(errno) << std::endl;
        }
    }
    printf("Unable to connect to the server\n");

    std::cerr << "Unable to connect to the server" << std::endl;
    freeaddrinfo(servinfo);
    return -1; // Return error status
}
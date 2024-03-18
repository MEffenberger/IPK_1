//
// Created by marek on 14.03.2024.
//

#include "StreamHandler.h"



void StreamHandler::add_to_epoll(int fd, uint32_t events) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
        perror("epoll_ctl: add");
        exit(EXIT_FAILURE);
    }
}

void StreamHandler::run_event_loop() {
    {
        // Create an epoll instance
        epoll_fd = epoll_create1(0);
        if (epoll_fd == -1) {
            perror("epoll_create1");
            exit(EXIT_FAILURE);
        }

        // Add the socket and STDIN_FILENO to the epoll instance
        add_to_epoll(sockfd, EPOLLIN); // EPOLLIN for readiness to read
        add_to_epoll(STDIN_FILENO, EPOLLIN);

        // Event loop
        std::vector<struct epoll_event> events(2); // 2 events for STDIN and socket

        while (true) {
            int n_events = epoll_wait(epoll_fd, events.data(), events.size(), -1);
            if (n_events == -1) {
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < n_events; ++i) {
                if (events[i].data.fd == sockfd) {
                    // Ready to read from the socket
                    // Read data, process it with protocol_fsm
                    // and call protocolHandler->process_server_message()
                    // to handle the message
                    // Ready to read from the socket
                    char buffer[1500];
                    std::string message;
                    ssize_t bytes_received;
                    bool message_complete = false;

                    while (!message_complete) {
                        memset(buffer, 0, sizeof(buffer));
                        bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

                        if (bytes_received < 0) {
                            perror("recv");
                            // Decide how to handle the recv error, maybe continue;
                        } else if (bytes_received == 0) {
                            std::cout << "Server closed the connection" << std::endl;
                            close(sockfd);
                            return; // Or break if you want to keep the event loop running
                        } else {
                            // Successfully received data
                            message.append(buffer, bytes_received);
                            // Check if we've received the end-of-message delimiter
                            if (message.find("\r\n") != std::string::npos) {
                                message_complete = true;
                            }
                        }
                    }
                    // Find the position of "\r\n" in the message
                    size_t pos = message.find("\r\n");
                    if (pos != std::string::npos) {
                        // If found, erase it from the message
                        message.erase(pos, 2);
                    }

                    // Pass the complete message to the protocol handler
                    protocolHandler->process_server_message(message);

                } else if (events[i].data.fd == STDIN_FILENO) {
                    // Ready to read from stdin
                    // Read input, send appropriate messages to the server
                    // and call protocolHandler->process_user_input()
                    // to handle the message
                    std::string input;
                    std::getline(std::cin, input);
                    protocolHandler->process_user_input(input);

                }
            }
        }
    }
}
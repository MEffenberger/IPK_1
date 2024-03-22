
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

    // Set stdin in non-blocking mode if not a terminal
    if (!isatty(STDIN_FILENO)) {
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        if (flags == -1) {
            perror("fcntl");
            exit(EXIT_FAILURE);
        }
        if (fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK) == -1) {
            perror("fcntl");
            exit(EXIT_FAILURE);
        }
    }

    // Create epoll file descriptor
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // Add the socket and stdin to epoll
    add_to_epoll(sockfd, EPOLLIN); // EPOLLIN for readiness to read
    add_to_epoll(STDIN_FILENO, EPOLLIN);

    std::vector<struct epoll_event> events(2);
    std::queue<std::string> userCommands; // Queue for user commands

    ProtocolHandler::ClientState state = ProtocolHandler::ClientState::READY_FOR_INPUT;

    while (true) {
        if (state == ProtocolHandler::ClientState::OVER) {
            close(sockfd);
            return;
        }

        int n_events = epoll_wait(epoll_fd, events.data(), events.size(), -1);
        if (n_events == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n_events; ++i) {
            if (events[i].data.fd == sockfd) {
                // Handle server input
                state = protocolHandler->process_server_message();

            } else if (events[i].data.fd == STDIN_FILENO) {
                // Handle user input
                std::string input;
                if (std::getline(std::cin, input)) { // Non-blocking read
                    userCommands.push(input);
                }
            }
        }

        if (state == ProtocolHandler::ClientState::WAITING_FOR_REPLY){
            continue;
        }

        // Process any queued user commands
        if (!userCommands.empty()) {
            std::string command = userCommands.front();
            userCommands.pop();
            state = protocolHandler->process_user_input(command);
        }
    }
}
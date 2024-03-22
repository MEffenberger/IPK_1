
#include "StreamHandler.h"
#include <poll.h>


void StreamHandler::run_event_loop() {
    // Array of pollfd structures
    std::vector<struct pollfd> fds(2);

    // Initialize the array for socket and stdin
    fds[0].fd = sockfd; // First file descriptor for the socket
    fds[0].events = POLLIN; // Check for ready to read

    fds[1].fd = STDIN_FILENO; // Second file descriptor for stdin
    fds[1].events = POLLIN; // Check for ready to read

    std::queue<std::string> userCommands; // Queue for user commands

    ProtocolHandler::ClientState state = ProtocolHandler::ClientState::READY_FOR_INPUT;

    printf("Starting event loop\n");
    while (true) {
        if (state == ProtocolHandler::ClientState::OVER) {
            close(sockfd);
            return;
        }

        printf("Waiting for events\n");

        int n_events = poll(fds.data(), fds.size(), -1); // -1 means wait indefinitely

        printf("Got %d events\n", n_events);

        if (n_events == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        if (n_events > 0) {
            // Check if the socket is ready to read
            if (fds[0].revents & POLLIN) {
                state = protocolHandler->process_server_message();
            }

            // Check if stdin is ready to read
            if (fds[1].revents & POLLIN) {
                std::string input;
                input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());
                if (std::getline(std::cin, input)) { // Blocking read
                    userCommands.push(input);
                } else {
                    // EOF
                    state = protocolHandler->process_user_input("/exit");
                }
            }
        }

        if (state == ProtocolHandler::ClientState::WAITING_FOR_REPLY){
            continue;
        }

        // Process any queued user commands
        while (!userCommands.empty() && state != ProtocolHandler::ClientState::WAITING_FOR_REPLY) {
            std::string command = userCommands.front();
            userCommands.pop();
            state = protocolHandler->process_user_input(command);
        }
    }
}

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
    int timeout_duration = -1;

    while (true) {
        if (state == ProtocolHandler::ClientState::OVER) {
            close(sockfd);
            return;
        }
        if (state == ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION) {
            timeout_duration = confirmation_timeout; // Set to the specified timeout duration
        } else {
            timeout_duration = -1; // Otherwise, wait indefinitely
        }


        int n_events = poll(fds.data(), fds.size(), timeout_duration);


        if (n_events == -1) {
            perror("poll");
            exit(EXIT_FAILURE);


        } else if (n_events == 0 && state == ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION) {
            // Timeout occurred
            if (retry_count > 0) {
                // Retry sending the message
                protocolHandler->resend_last_message();
                retry_count--;
            } else {
                // Max retries reached
                std::cerr << "Max retries reached, giving up.\n";
                state = ProtocolHandler::ClientState::OVER;
            }


        } else if (n_events > 0) {


            // Check if the socket is ready to read
            if (fds[0].revents & POLLIN) {
                state = protocolHandler->process_server_message();
            }


            if (state == ProtocolHandler::ClientState::WAITING_FOR_REPLY || state == ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION) {
                continue;
            }


            // Check if stdin is ready to read
            if (fds[1].revents & POLLIN) {
                std::string input;
                if (std::getline(std::cin, input)) { // Blocking read
                    // Strip newline character if necessary
                    if (input.empty()) {
                        continue;
                    }
                    input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());
                    state = protocolHandler->process_user_input(input);
                } else {
                    // EOF or other read error
                    state = protocolHandler->process_user_input("/exit");
                }
            }
        }
    }
}
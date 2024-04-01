/**
 * @file StreamHandler.cpp
 * Implementation of the StreamHandler class.
 * Main purpose of this class is to run the event loop and communicate with the interface
 *
 * @Author Marek Effenberger
 */



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

    // Initial state
    ProtocolHandler::ClientState state = ProtocolHandler::ClientState::READY_FOR_INPUT;
    int timeout_duration = -1;

    // Flag to check if the signal was interrupted
    bool first_time_interrupted = true;

    // Number of retries
    int retrans = retry_count;

    // Run the event loop
    while (true) {


        // Ending the connection
        if (state == ProtocolHandler::ClientState::OVER) {
            close(sockfd);
            return;
        }

        // Set the timeout duration
        if (state == ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION) {
            timeout_duration = confirmation_timeout; // Set to the specified timeout duration
        } else {
            timeout_duration = -1; // Otherwise, wait indefinitely
        }

        // Poll the file descriptors if the state is waiting for confirmation block the stdin
        int nfds = ((state == ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION) || (state == ProtocolHandler::ClientState::WAITING_FOR_REPLY)) ? 1 : 2;
        int n_events = poll(fds.data(), nfds, timeout_duration);

        // Check if the signal was interrupted, if so, send bye
        if (interrupted && first_time_interrupted) {
            state = protocolHandler->process_user_input("/exit");
            first_time_interrupted = false;
            continue;
        }

        // Check if the signal was interrupted, if so, send bye
        if (n_events == -1) {
            perror("poll");
            exit(EXIT_FAILURE);

        // The timeout occurred, resend the message
        } else if (n_events == 0 && state == ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION) {
            // Timeout occurred
            if (retry_count > 0) {
                // Retry sending the message
                protocolHandler->resend_last_message();
                retry_count--;
                state = ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION;
                continue;

            } else {
                // Max retries reached
                state = ProtocolHandler::ClientState::OVER;
                continue;
            }

        // The socket is ready to read or stdin is ready to read
        } else if (n_events > 0) {


            // Check if the socket is ready to read
            if (fds[0].revents & POLLIN) {
                state = protocolHandler->process_server_message();
                if (state != ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION){
                    retry_count = retrans;
                    continue;
                }
                continue;
            }

            // Do not read the stdin if the state is waiting for reply or confirmation
            if (state == ProtocolHandler::ClientState::WAITING_FOR_REPLY || state == ProtocolHandler::ClientState::WAITING_FOR_CONFIRMATION) {
                continue;
            }


            // Check if stdin is ready to read
            if (fds[1].revents & (POLLIN | POLLHUP)) {
                std::string input;
                if (std::getline(std::cin, input)) {
                    if (input.empty()) {
                        continue;
                    }
                    state = protocolHandler->process_user_input(input);
                } else {
                    // EOF or other read error
                    state = protocolHandler->process_user_input("/exit");
                }
            }
        }
    }
}

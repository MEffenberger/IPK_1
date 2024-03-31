/**
 * @file main.cpp
 *
 * Main file of the client. calling the parser, connecting to the server and running the event loop
 *
 * @Author Marek Effenberger
 */

// Eternal Glory to the owner of this site right here https://beej.us/guide/bgnet/html/ and his guide to network programming
// Everything as of writing this comment is based on his guide

#include "Parser.h"
#include "Config.h"
#include "connectToServer.h"
#include "StreamHandler.h"
#include "interruptSignal.h"

/**
 * Main function
 * @param argc
 * @param argv
 * @return 0 if everything is ok
 */
int main(int argc, char *argv[]) {

    // Setting up the config and validating arguments
    Config config;
    Parser::parse(argc, argv, config);
    config.Validate();
    // Getting  the file descriptor
    int fdconnect = connectToServer(config);
    // If the connection failed, exit
    if (fdconnect == -1) {
        std::cerr << "Unable to connect to the server" << std::endl;
        exit(EXIT_FAILURE);
    }
    // Setting up the signal handler
    std::signal(SIGINT, signal_handler);
    // Setting up the stream handler and running the event loop
    StreamHandler streamHandler(fdconnect, config.protocol, config.max_udp_retransmissions, config.udp_confirmation_timeout, getServerAddress(config));
    streamHandler.run_event_loop();

    // Hooray
    return 0;
}
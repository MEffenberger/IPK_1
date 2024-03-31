/**
 * @file Config.cpp
 * Config class implementation
 *
 * This class is used to store the configuration of the client
 *
 * @Author Marek Effenberger
 */

#include "Config.h"

// Default constructor
Config::Config() : protocol(""),
                   server_ip(""),
                   server_port(4567),
                   udp_confirmation_timeout(250),
                   max_udp_retransmissions(3) {}

// Validate the configuration
void Config::Validate() const {
    // Check if the protocol is valid
    if (protocol != "tcp" && protocol != "udp") {
        if (protocol.empty()) {
            std::cerr << "Protocol is required" << std::endl;
        } else {
            std::cerr << "Invalid protocol provided " << protocol << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    // Check if the server IP is valid
    if (std::string(server_ip).empty()){
        std::cerr << "Server IP is required" << std::endl;
        exit(EXIT_FAILURE);
    }
}
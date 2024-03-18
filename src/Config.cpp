//
// Created by marek on 14.03.2024.
//

#include "Config.h"

Config::Config() : protocol(""),
                   server_ip(""),
                   server_port(4567),
                   udp_confirmation_timeout(250),
                   max_udp_retransmissions(3) {}

void Config::Validate() const {
    if (protocol != "tcp" && protocol != "udp") {
        if (protocol.empty()) {
            std::cerr << "Protocol is required" << std::endl;
        } else {
            std::cerr << "Invalid protocol provided " << protocol << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    if (std::string(server_ip).empty()){
        std::cerr << "Server IP is required" << std::endl;
        exit(EXIT_FAILURE);
    }
}
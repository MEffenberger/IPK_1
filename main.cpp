#include <iostream>
#include <string>
#include "Parser.h"
#include "Config.h"

// Etetnal Glory to the owner of this site right here https://beej.us/guide/bgnet/html/ and his guide to network programming
// Everything as of writing this comment is based on his guide

int main(int argc, char *argv[]) {


    Config config;
    Parser::parse(argc, argv, config);
    config.Validate();

    if (config.protocol == "tcp") {
        std::cout << "Using TCP protocol" << std::endl;
        tcp_protocol_connect(config);
    } else if (config.protocol == "udp") {
        std::cout << "Using UDP protocol" << std::endl;
    }

    std::cout << "Protocol: " << config.protocol << std::endl;
    std::cout << "Server IP: " << config.server_ip << std::endl;
    std::cout << "Server Port: " << config.server_port << std::endl;
    std::cout << "UDP Confirmation Timeout: " << config.udp_confirmation_timeout << std::endl;
    std::cout << "Maximum UDP Retransmissions: " << static_cast<int>(config.max_udp_retransmissions) << std::endl;

    return 0;
}
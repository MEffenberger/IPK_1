//
// Created by marek on 14.03.2024.
//

#include "Parser.h"

void print_help(const std::string& argv) {
    std::cout << "Usage: " << argv << " [options]\n";
    std::cout << "Options:\n";
    std::cout << "  -t <protocol>          Protocol to use (tcp or udp)\n";
    std::cout << "  -s <server_ip>         Server IP address\n";
    std::cout << "  -p <server_port>       Server port number               Default: 4567\n";
    std::cout << "  -d <timeout>           UDP confirmation timeout         Default: 250\n";
    std::cout << "  -r <retransmissions>   Maximum UDP retransmissions      Default: 3\n";
    exit(EXIT_SUCCESS);
}

void Parser::parse(int argc, char *argv[], Config& config) {
    int c;
    while ((c = getopt(argc, argv, "t:s:p:d:r:h")) != -1) {
        switch (c) {
            case 't':
                config.protocol = optarg;
                break;
            case 's':
                config.server_ip = optarg;
                break;
            case 'p':
                config.server_port = (uint16_t) atoi(optarg);
                break;
            case 'd':
                config.udp_confirmation_timeout = (uint16_t) atoi(optarg);
                break;
            case 'r':
                config.max_udp_retransmissions = (uint8_t) atoi(optarg);
                break;
            case 'h':
                print_help(argv[0]);
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [options]\n";
                exit(EXIT_FAILURE);
        }
    }
}

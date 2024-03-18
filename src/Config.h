//
// Created by marek on 14.03.2024.
//

#ifndef IPK_1_CONFIG_H
#define IPK_1_CONFIG_H

#include <string>
#include <cstdlib>
#include <iostream>


class Config {
public:
    std::string protocol;
    const char *server_ip;
    uint16_t server_port;
    uint16_t udp_confirmation_timeout;
    uint8_t max_udp_retransmissions;        /////// POZOR NA TOTO
    // IPv4 address: 4 bytes

    Config();

    void Validate() const;

};


#endif //IPK_1_CONFIG_H

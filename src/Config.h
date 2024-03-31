/**
 * @file Config.h
 * Config class implementation
 *
 * This class is used to store the configuration of the client
 *
 * @Author Marek Effenberger
 */

#ifndef IPK_1_CONFIG_H
#define IPK_1_CONFIG_H

#include <string>
#include <cstdlib>
#include <iostream>

/**
 * Config class
 *
 * This class is used to store the configuration of the client
 */
class Config {

public:

    std::string protocol;
    const char *server_ip;
    uint16_t server_port;
    uint16_t udp_confirmation_timeout;
    uint8_t max_udp_retransmissions;

    /**
     * Default constructor
     */
    Config();

    /**
     * Validate the configuration
     */
    void Validate() const;

};


#endif //IPK_1_CONFIG_H

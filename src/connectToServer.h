/**
 * @file connectToServer.h
 * Declarations of connectToServer function
 * Connects to the server using the provided configuration
 * Returns the socket file descriptor
 *
 * @Author Marek Effenberger
 */

#ifndef IPK_1_CONNECT_H
#define IPK_1_CONNECT_H

#include "Config.h"
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * Connect to the server
 * @param config
 * @return socket file descriptor
 */
int connectToServer(const Config& config);

/**
 * Get server address
 * @param config
 * @return server address structure
 */
struct sockaddr_in getServerAddress(const Config& config);

#endif //IPK_1_CONNECT_H

//
// Created by marek on 14.03.2024.
//

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


int connectToServer(const Config& config);
struct sockaddr_in getServerAddress(const Config& config);

#endif //IPK_1_CONNECT_H

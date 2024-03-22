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


int connectToServer(const Config& config);

#endif //IPK_1_CONNECT_H

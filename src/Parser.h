//
// Created by marek on 14.03.2024.
//

#ifndef IPK_1_PARSER_H
#define IPK_1_PARSER_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <getopt.h>
#include "Config.h"

void print_help(const std::string& argv);

class Parser {
public:
    static void parse(int argc, char *argv[], Config& config);
};


#endif //IPK_1_PARSER_H

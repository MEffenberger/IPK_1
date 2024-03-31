/**
 * @file Parser.h
 *
 * This file contains the declaration of the Parser class
 *
 * @Author Marek Effenberger
 */

#ifndef IPK_1_PARSER_H
#define IPK_1_PARSER_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <getopt.h>
#include "Config.h"

/**
 * Print help message
 * @param argv
 */
void print_help(const std::string& argv);

/**
 * Parse the command line arguments
 */
class Parser {
public:
    /**
     * Parse the command line arguments
     * @param argc
     * @param argv
     * @param config
     */
    static void parse(int argc, char *argv[], Config& config);
};


#endif //IPK_1_PARSER_H

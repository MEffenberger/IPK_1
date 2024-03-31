/**
 * @file ClientOutput.h
 * Implementation of the ClientOutput class
 * This class is responsible for printing messages to the console
 * for the client.
 *
 * @Author Marek Effenberger
 */

#ifndef IPK_1_CLIENTOUTPUT_H
#define IPK_1_CLIENTOUTPUT_H

#include <string>
#include <iostream>

class ClientOutput {

public:

/**
 * Print message received from the server
 * @param Username
 * @param message
 */
void message_from_server(const std::string&Username, const std::string& message);

/**
 * Print internal error message
 * @param message
 */
void internal_error_message(const std::string& message);

/**
 * Print help message
 */
void print_help();

/**
 * Print error message from the server
 * @param Username
 * @param message
 */
void error_from_server(const std::string&Username, const std::string& message);

/**
 * Print reply success message
 * @param message
 */
void reply_success(const std::string& message);

/**
 * Print reply error message
 * @param message
 */
void reply_error(const std::string& message);

};


#endif //IPK_1_CLIENTOUTPUT_H

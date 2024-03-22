//
// Created by marek on 21.03.2024.
//

#ifndef IPK_1_CLIENTOUTPUT_H
#define IPK_1_CLIENTOUTPUT_H

#include <string>
#include <iostream>

class ClientOutput {

public:


void message_from_server(const std::string&Username, const std::string& message);
void internal_error_message(const std::string& message);
void print_help();
void error_from_server(const std::string&Username, const std::string& message);
void reply_success(const std::string& message);
void reply_error(const std::string& message);

};


#endif //IPK_1_CLIENTOUTPUT_H

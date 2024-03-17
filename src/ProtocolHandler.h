//
// Created by marek on 16.03.2024.
//

#ifndef IPK_1_PROTOCOLHANDLER_H
#define IPK_1_PROTOCOLHANDLER_H

#include <string>


class ProtocolHandler {
public:
    virtual void process_server_message(const std::string& message) = 0;
    virtual void process_user_input(const std::string& message) = 0;
    virtual ~ProtocolHandler() = default;
};


#endif //IPK_1_PROTOCOLHANDLER_H

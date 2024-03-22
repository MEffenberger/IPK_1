//
// Created by marek on 16.03.2024.
//

#ifndef IPK_1_PROTOCOLHANDLER_H
#define IPK_1_PROTOCOLHANDLER_H

#include <string>


class ProtocolHandler {
public:
    enum class ClientState {
        WAITING_FOR_REPLY,
        READY_FOR_INPUT,
        OVER
    };
    virtual ClientState process_server_message() = 0;
    virtual ClientState process_user_input(const std::string& message) = 0;
    virtual ~ProtocolHandler() = default;
};


#endif //IPK_1_PROTOCOLHANDLER_H

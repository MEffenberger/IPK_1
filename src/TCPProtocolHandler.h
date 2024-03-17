//
// Created by marek on 16.03.2024.
//

#ifndef IPK_1_TCPPROTOCOLHANDLER_H
#define IPK_1_TCPPROTOCOLHANDLER_H

#include "ProtocolHandler.h"


class TCPProtocolHandler : public ProtocolHandler {
private:
    // ProtocolStateMachine protocol_fsm; // The protocol state machine
    enum class State {
        START,
        AUTH,
        OPEN,
        ERROR,
        END
    };


    State currentState;
    std::string displayName;

    void write_error_message(const std::string& message);

    std::string authorize_validate_send(const std::string& message);
    std::string join_validate_send(const std::string& message);
    std::string message_validate_send(const std::string& message);
    std::string bye_validate_send(const std::string& message);
    std::string error_validate_send(const std::string& message);

    std::vector<std::string> split_message(const std::string& message);
    void validate_id(const std::string& id);
    void validate_secret(const std::string& secret);
    void validate_content(const std::string& content);
    void validate_dname(const std::string& dname);

    void print_help();
    void rename(const std::string& dname);

public:
    TCPProtocolHandler() : currentState(State::START) {}


    void process_server_message(const std::string& message) override;

    void process_user_input(const std::string& message) override;
};


#endif //IPK_1_TCPPROTOCOLHANDLER_H

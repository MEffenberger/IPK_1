//
// Created by marek on 16.03.2024.
//

#ifndef IPK_1_UDPPROTOCOLHANDLER_H
#define IPK_1_UDPPROTOCOLHANDLER_H

#include "ProtocolHandler.h"

class UDPProtocolHandler : public ProtocolHandler {
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

public:
    UDPProtocolHandler() : currentState(State::START) {}


    void process_server_message(const std::string& message) override {
        // Parse the message and call the appropriate handler
        // based on the protocol and the Mealy machine logic
    }

    void process_user_input(const std::string& message) override {
        // Parse the message and call the appropriate handler
        // based on the protocol and the Mealy machine logic
    }
};


#endif //IPK_1_UDPPROTOCOLHANDLER_H

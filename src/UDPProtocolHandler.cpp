//
// Created by marek on 26.03.2024.
//

#include "UDPProtocolHandler.h"

ProtocolHandler::ClientState UDPProtocolHandler::process_server_message() {
    // Receive the message
    return ProtocolHandler::ClientState::WAITING_FOR_REPLY;
}

ProtocolHandler::ClientState UDPProtocolHandler::process_user_input(const std::string& message) {
    // Send the message
    return ProtocolHandler::ClientState::WAITING_FOR_REPLY;
}
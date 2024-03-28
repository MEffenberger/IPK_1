//
// Created by marek on 26.03.2024.
//

#ifndef VUT_IPK_UDPPROTOCOLHANDLER_H
#define VUT_IPK_UDPPROTOCOLHANDLER_H

#include "ProtocolHandler.h"
#include "FSMValidate.h"
#include "ClientOutput.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <cstring>
#include "UDPMessageValidator.h"


class UDPProtocolHandler : public ProtocolHandler {
private:
    int sockfd; // Socket file descriptor
    uint8_t retry_count;
    uint16_t confirmation_timeout;
    FSMValidate fsm;
    ClientOutput clientOutput;
    struct sockaddr_in server_address;

    std::vector<uint16_t> confirmed_messages;
    uint16_t awaited_conirm_id;
    uint16_t awaited_reply_id;
    uint16_t renamer3000;
    bool waiting_for_reply = false;
    std::vector<uint8_t> last_message;
    bool awaiting_over = false;

    UDPMessageValidator messageValidator;

    void send_message(const std::vector<uint8_t>& message);

public:
    UDPProtocolHandler(int fd, uint8_t retry_count, uint16_t confirmation_timeout, struct sockaddr_in serverAddr) : sockfd(fd), retry_count(retry_count), confirmation_timeout(confirmation_timeout), fsm(),
                                                                                                                    server_address(serverAddr) {}
    void resend_last_message();
    ProtocolHandler::ClientState process_server_message() override;

    ProtocolHandler::ClientState process_user_input(const std::string& message) override;
};


#endif //VUT_IPK_UDPPROTOCOLHANDLER_H

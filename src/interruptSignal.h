//
// Created by marek on 29.03.2024.
//

#ifndef VUT_IPK_INTERRUPTSIGNAL_H
#define VUT_IPK_INTERRUPTSIGNAL_H

#include <csignal>
#include <atomic>

extern std::atomic<bool> interrupted;  // Use extern keyword here
void signal_handler(int signal);

#endif //VUT_IPK_INTERRUPTSIGNAL_H

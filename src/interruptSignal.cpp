//
// Created by marek on 29.03.2024.
//

#include "interruptSignal.h"

std::atomic<bool> interrupted(false);  // Definition goes here, without extern

void signal_handler(int signal) {
    if (signal == SIGINT) {
        interrupted = true;
    }
}
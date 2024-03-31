/**
 * @file interruptSignal.cpp
 * Implementation of interruptSignal.h
 *
 * Handles the signal interrupt
 *
 * @Author Marek Effenberger
 */

#include "interruptSignal.h"

// Global variable to store the interrupt status
std::atomic<bool> interrupted(false);

// Signal handler
void signal_handler(int signal) {
    if (signal == SIGINT) {
        interrupted = true;
    }
}
/**
 * @file interruptSignal.h
 * Declaration of interruptSignal.h
 *
 * Handles the signal interrupt
 *
 * @Author Marek Effenberger
 */

#ifndef VUT_IPK_INTERRUPTSIGNAL_H
#define VUT_IPK_INTERRUPTSIGNAL_H

#include <csignal>
#include <atomic>

/// Global variable to store the interrupt status
extern std::atomic<bool> interrupted;
/**
 * Signal handler
 * @param signal
 */
void signal_handler(int signal);

#endif //VUT_IPK_INTERRUPTSIGNAL_H

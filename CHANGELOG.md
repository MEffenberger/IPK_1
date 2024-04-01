# Changelog

All notable changes to this project will be documented in this file.

### Mar 22, 2024, 7:53 PM 
Implementation of TCP basic functionality. Currently not supporting UDP protocol.
Still utilizing a queue for incoming messages.

### Mar 31, 2024, 8:00 PM, v1.0.0
The implementation of the UDP protocol is now supported. The queue has been removed as OS buffers the input.
As of now the only downside might be wrong time handling in the re-sends.

Main Downgrade as of the version 1.0.0 is personal dissatisfaction with the code quality.
On the other hand the tested scenarios have shown that the implementation is working, 
except of a correct timeout handling. If message is received, not being a confirmation, the client waits
for the original timeout, not the original timeout minus the time for the non-confirmation message to arrive.
This could be fixed by proper use of 'chrono' library.
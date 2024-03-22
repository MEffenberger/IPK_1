#include "Parser.h"
#include "Config.h"
#include "connectToServer.h"
#include "StreamHandler.h"


// Eternal Glory to the owner of this site right here https://beej.us/guide/bgnet/html/ and his guide to network programming
// Everything as of writing this comment is based on his guide

int main(int argc, char *argv[]) {


    Config config;
    Parser::parse(argc, argv, config);
    printf("Parsed config\n");
    config.Validate();
    printf("Validated config\n");
    int fdconnect = connectToServer(config);
    if (fdconnect == -1) {
        std::cerr << "Unable to connect to the server" << std::endl;
        exit(EXIT_FAILURE);
    }
    printf("Connected to server\n");

    StreamHandler streamHandler(fdconnect, config.protocol);
    streamHandler.run_event_loop();

    return 0;
}
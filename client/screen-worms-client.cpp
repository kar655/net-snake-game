#include <iostream>
#include "arguments_parser_client.h"
#include "steering.h"
#include "client_connection.h"

struct start_config {
    uint_fast32_t maxx;
    uint_fast32_t maxy;
    std::vector<std::string> playerNames;

    start_config()
            : maxx(50), maxy(30),
              playerNames({
                                  "FirstPlayer",
                                  "SecondPlayer",
                          }) {}
};

void runGUIConnection(ArgumentsParserClient const &argumentsParser) {
    start_config startConfig;

    ClientToGUIConnection guiConnection(argumentsParser.getGuiServer(),
                                        argumentsParser.getGuiPort());

    guiConnection.initialMessage(startConfig.maxx, startConfig.maxy,
                                 startConfig.playerNames);

    guiConnection.startReading();
}

void runServerConnection(ArgumentsParserClient const &argumentsParser) {
    start_config startConfig;

    ClientToServerConnection serverConnection(argumentsParser.getGameServer(),
                                              argumentsParser.getServerPort());

    serverConnection.sendClientMessage();
    serverConnection.receiveServerMessage();
}

int main(int argc, char *argv[]) {
    std::cout << "Hello" << std::endl;

    uint_fast64_t session_id = time(nullptr);
    std::cout << "Current session_id = " << session_id << std::endl;

    ArgumentsParserClient const argumentsParser(argc, argv);

    std::cout << argumentsParser << std::endl;

//    stopBuffering();
//    while (true) {
//        parseMove();
//    }

//    runGUIConnection(argumentsParser);
    runServerConnection(argumentsParser);

    return 0;
}

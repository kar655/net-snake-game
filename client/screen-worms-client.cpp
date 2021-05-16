#include <iostream>
#include <thread>
#include "arguments_parser_client.h"
#include "steering.h"
#include "client_connection.h"
#include "client_messenger.h"

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

void runClientMessenger(ArgumentsParserClient const &argumentsParser) {
    ClientMessage message;
    message.session_id = 2137;
    message.turn_direction = 2;
    message.next_expected_event_no = 32;

    start_config startConfig;

    ClientToServerConnection serverConnection(argumentsParser.getGameServer(),
                                              argumentsParser.getServerPort());

    ClientMessenger clientMessenger(message);

    std::cout << "Starting running" << std::endl;
    clientMessenger.run(serverConnection);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    clientMessenger.stopRunning();
    std::cout << "Stop running" << std::endl;
}

int main(int argc, char *argv[]) {
    ArgumentsParserClient const argumentsParser(argc, argv);

    std::cout << argumentsParser << std::endl;

//    runGUIConnection(argumentsParser);
//    runServerConnection(argumentsParser);
    runClientMessenger(argumentsParser);

    return 0;
}

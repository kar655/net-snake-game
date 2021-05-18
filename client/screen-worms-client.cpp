#include <iostream>
#include <thread>
#include "arguments_parser_client.h"
#include "steering.h"
#include "client_connection.h"
#include "client_messenger.h"
#include <thread>
#include <chrono>

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

void runGUIMessengerConnection(ArgumentsParserClient const &argumentsParser) {
//    ClientMessage message;
//    message.session_id = 2137;
//    message.turn_direction = 0;
//    message.next_expected_event_no = 32;
//
//    start_config startConfig;
//
//    ClientToServerConnection serverConnection(argumentsParser.getGameServer(),
//                                              argumentsParser.getServerPort());
//    ClientMessenger clientMessenger(message);
//    clientMessenger.run(serverConnection);
//
//    ClientToGUIConnection guiConnection(argumentsParser.getGuiServer(),
//                                        argumentsParser.getGuiPort(),
//                                        message);
//
//    guiConnection.initialMessage(startConfig.maxx, startConfig.maxy,
//                                 startConfig.playerNames);
//
//    guiConnection.startReading();
//    clientMessenger.stopRunning();
}

void runServerConnection(ArgumentsParserClient const &argumentsParser) {
//    ClientToServerConnection serverConnection(argumentsParser.getGameServer(),
//                                              argumentsParser.getServerPort());
//
//    serverConnection.sendClientMessage();
////    serverConnection.receiveServerMessage();
//    serverConnection.receiveEvent();
}

void runServerAndGuiConnection(ArgumentsParserClient const &argumentsParser) {
    ClientMessage message;

    ClientToGUIConnection guiConnection(argumentsParser.getGuiServer(),
                                        argumentsParser.getGuiPort(),
                                        message,
                                        "First");

    ClientToServerConnection serverConnection(argumentsParser.getGameServer(),
                                              argumentsParser.getServerPort());

    ClientMessenger clientMessenger(message);
    clientMessenger.run(serverConnection);

    guiConnection.startReading();


//    for (int i = 0; i < 10; i++) {
//        serverConnection.receiveEvent(guiConnection, message);
//    }
    serverConnection.run(guiConnection, message);


    // todo reading events from main thread and stop after game over
    std::this_thread::sleep_for(std::chrono::seconds(20));
//    clientMessenger.stopRunning();
}

int main(int argc, char *argv[]) {
    ArgumentsParserClient const argumentsParser(argc, argv);

    std::cout << argumentsParser << std::endl;

//    runGUIMessengerConnection(argumentsParser);
//    runServerConnection(argumentsParser);
    runServerAndGuiConnection(argumentsParser);

    return 0;
}

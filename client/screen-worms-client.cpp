#include <iostream>
#include "arguments_parser_client.h"
#include "steering.h"
#include "client_connection.h"
#include "client_messenger.h"
#include <thread>
#include <chrono>


void runGUIMessengerConnection(ArgumentsParserClient const &argumentsParser) {
    ClientMessage message;
    message.session_id = 2137;
    message.turn_direction = 0;
    message.next_expected_event_no = 32;

    ClientToServerConnection serverConnection(argumentsParser.getGameServer(),
                                              argumentsParser.getServerPort());
    ClientMessenger clientMessenger(message, serverConnection);

    ClientToGUIConnection guiConnection(argumentsParser.getGuiServer(),
                                        argumentsParser.getGuiPort(),
                                        message,
                                        "First");

    guiConnection.initialMessage(100, 80,
                                 {"First"});

    guiConnection.startReading();
}

void runServerAndGuiConnection(ArgumentsParserClient const &argumentsParser) {
    ClientMessage message;

    ClientToGUIConnection guiConnection(argumentsParser.getGuiServer(),
                                        argumentsParser.getGuiPort(),
                                        message,
                                        "First");

    ClientToServerConnection serverConnection(argumentsParser.getGameServer(),
                                              argumentsParser.getServerPort());

    ClientMessenger clientMessenger(message, serverConnection);

    guiConnection.startReading();

    while (!serverConnection.receivedGameOver()) {
        serverConnection.receiveEvent(guiConnection, message);
    }
}

int main(int argc, char *argv[]) {
    ArgumentsParserClient const argumentsParser(argc, argv);
    std::cout << argumentsParser << std::endl;

//    runGUIMessengerConnection(argumentsParser);
    runServerAndGuiConnection(argumentsParser);

    return 0;
}

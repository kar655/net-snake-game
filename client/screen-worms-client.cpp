#include <iostream>
#include "arguments_parser_client.h"
#include "steering.h"
#include "client_connection.h"
#include "client_messenger.h"
#include <thread>
#include <chrono>


void runServerAndGuiConnection(ArgumentsParserClient const &argumentsParser) {
    std::cout << "sizeof(ClientMessage) = " << sizeof(ClientMessage) << std::endl;
    ClientMessageWrapper message(argumentsParser.getPlayerName());

    ClientToGUIConnection guiConnection(argumentsParser, message);

    ClientToServerConnection serverConnection(argumentsParser.getGameServer(),
                                              argumentsParser.getServerPort());

    ClientMessenger clientMessenger(message, serverConnection);

    guiConnection.startReading();

//    while (!serverConnection.receivedGameOver()) {
    while (true) {
        serverConnection.receiveEvent(guiConnection, message);
    }
}

int main(int argc, char *argv[]) {
    ArgumentsParserClient const argumentsParser(argc, argv);
    std::cout << argumentsParser << std::endl;

    runServerAndGuiConnection(argumentsParser);

    return 0;
}

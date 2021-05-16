#include <iostream>
#include "server_connection.h"
#include "../common/messages.h"
#include "arguments_parser_server.h"

void debugSizes() {
    std::cout
    << "uint_fast8_t=" << sizeof(uint_fast8_t) << std::endl
    << "uint_fast16_t=" << sizeof(uint_fast16_t) << std::endl
    << "uint_fast32_t=" << sizeof(uint_fast32_t) << std::endl
    << "uint_fast64_t=" << sizeof(uint_fast64_t) << std::endl
    << std::endl
    << "uint8_t=" << sizeof(uint8_t) << std::endl
    << "uint16_t=" << sizeof(uint16_t) << std::endl
    << "uint32_t=" << sizeof(uint32_t) << std::endl
    << "uint64_t=" << sizeof(uint64_t) << std::endl;
}

void runClientConnection() {
    uint_fast16_t port = 1111;
    ServerToClientConnection clientConnection(port);

    clientConnection.receiveClientMessage();
    clientConnection.sendServerMessage("Ja serwer");
}

void runManager(ArgumentsParserServer const &argumentParser) {
    ServerConnectionManager manager(argumentParser.getPort());
    manager.waitForNewClient();
    manager.closeAllConnections();
}

void runEventsSender(ArgumentsParserServer const &argumentParser) {
    ServerToClientConnection clientConnection(argumentParser.getPort());

    auto *event = new EventPlayerEliminated;
    size_t eventLength = sizeof(EventPlayerEliminated);
//    std::cout << "sizeof(event) = " << sizeof(event) << std::endl;
    event->len = 5;
    event->event_no = 132;
    event->cec32 = 32544231;

    std::cout << event << std::endl;
    std::cout << *event << std::endl;

    clientConnection.receiveClientMessage();
//    clientConnection.sendServerMessage("Ja serwer");
    clientConnection.sendEvent(event, eventLength);
}

int main(int argc, char *argv[]) {
//    debugSizes();

    ArgumentsParserServer const argumentParser(argc, argv);

    std::cout << argumentParser << std::endl;

//    runClientConnection();
//    runManager(argumentParser);
    runEventsSender(argumentParser);

    return 0;
}

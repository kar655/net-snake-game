#include <iostream>
#include "server_connection.h"
#include "../common/messages.h"
#include "arguments_parser_server.h"

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

    Event event;
    event.len = 3;
    event.event_no = 132;
    event.event_no = PIXEL;
    event.cec32 = 32544231;

    clientConnection.receiveClientMessage();
//    clientConnection.sendServerMessage("Ja serwer");
    clientConnection.sendEvent(event);
}

int main(int argc, char *argv[]) {
    std::cout << "Hello" << std::endl;

    ArgumentsParserServer const argumentParser(argc, argv);

    std::cout << argumentParser << std::endl;

//    runClientConnection();
//    runManager(argumentParser);
    runEventsSender(argumentParser);

    return 0;
}

#include <iostream>
#include "server_connection.h"
#include "arguments_parser_server.h"

void runClientConnection() {
    uint_fast16_t port = 1111;
    ServerToClientConnection clientConnection(port);

    clientConnection.receiveClientMessage();
    clientConnection.sendServerMessage("Ja serwer");
}

int main(int argc, char *argv[]) {
    std::cout << "Hello" << std::endl;

    ArgumentsParserServer const argumentParser(argc, argv);

    std::cout << argumentParser << std::endl;

//    runClientConnection();

    return 0;
}

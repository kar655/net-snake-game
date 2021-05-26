#include <iostream>

#include "server_connection.h"
#include "arguments_parser_server.h"
#include "game_state.h"

void runMultiClientConnection(ArgumentsParserServer const &argumentParser) {
    GameState gameState(argumentParser);

    /// ServerToClientConnection nasluchuje na wiadomosci
    /// gdy zobaczy ze juz taki klient jest to jego watkowi przekazuje prace
    /// gdy nie ma to tworzy nowy ClientHandler ktory obudowywuje watek
    ///  i podaje mu refencje na direction z gameState
    ServerToClientConnection clientConnection(gameState, argumentParser.getPort());
    clientConnection.run();

    while (true) {
        gameState.waitForClients();

        gameState.startGame();
        gameState.playGame();
        gameState.gameOver();
    }
}

int main(int argc, char *argv[]) {
    ArgumentsParserServer const argumentParser(argc, argv);


    std::cout << argumentParser << std::endl;

    runMultiClientConnection(argumentParser);
    return 0;
}

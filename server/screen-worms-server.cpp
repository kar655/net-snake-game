#include <iostream>
#include <chrono>
#include <thread>

#include "server_connection.h"
#include "arguments_parser_server.h"
#include "game_state.h"

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

void debugStructSizes() {
    std::cout
            << "sizeof(EventNewGame) = " << sizeof(EventNewGame) << std::endl
            << "sizeof(EventPixel) = " << sizeof(EventPixel) << std::endl
            << "sizeof(EventPlayerEliminated) = " << sizeof(EventPlayerEliminated) << std::endl
            << "sizeof(EventGameOver) = " << sizeof(EventGameOver) << std::endl;
}


//void runClientMessageReader(ArgumentsParserServer const &argumentParser) {
//    GameState gameState(argumentParser);
//    gameState.startGame();
//
//    ServerToClientConnection clientConnection(gameState, argumentParser.getPort(),
//                                              gameState.getDirection());
//    clientConnection.run();
//
//
//    for (int i = 0; i < 30; ++i) {
//        gameState.roundsForSecond();
//    }
//
//    gameState.gameOver();
//}

void runMultiClientConnection(ArgumentsParserServer const &argumentParser) {
    GameState gameState(argumentParser);

    /// ServerToClientConnection nasluchuje na wiadomosci
    /// gdy zobaczy ze juz taki klient jest to jego watkowi przekazuje prace
    /// gdy nie ma to tworzy nowy ClientHandler ktory obudowywuje watek
    ///  i podaje mu refencje na direction z gameState
    ServerToClientConnection clientConnection(gameState, argumentParser.getPort());
    clientConnection.run();

    gameState.waitForClients();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    gameState.startGame();
    for (int i = 0; i < 10; ++i) {
        gameState.roundsForSecond();
    }

    gameState.gameOver();
}

int main(int argc, char *argv[]) {
//    debugSizes();
//    debugStructSizes();

    ArgumentsParserServer const argumentParser(argc, argv);

    std::cout << argumentParser << std::endl;

//    runClientMessageReader(argumentParser);
    runMultiClientConnection(argumentParser);

    return 0;
}

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

void debugCrc() {
    const uint8_t xd[4] = {116, 101, 115, 116};
    std::cout << "3632233996 == " << ControlSum::crc32Check(xd, 4) << std::endl;


    uint32_t const event_no = 69;
    uint8_t const player_number = 2;
    uint32_t const x = 20;
    uint32_t const y = 40;

    std::cout << "sizeof(EventPixel)=" << sizeof(EventPixel) << std::endl;
    EventPixel event(event_no, player_number, x, y);
    std::cout << event << std::endl;

    event.toBigEndian();
    std::cout << event << std::endl;

    event.fromBigEndian();
    std::cout << event << std::endl;

    exit(0);
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

    while (true) {
        gameState.waitForClients();

        gameState.startGame();
        gameState.playGame();
        gameState.gameOver();
    }
}

int main(int argc, char *argv[]) {
//    debugSizes();
//    debugStructSizes();
//    debugCrc();

    ArgumentsParserServer const argumentParser(argc, argv);


    std::cout << argumentParser << std::endl;

//    runClientMessageReader(argumentParser);
    runMultiClientConnection(argumentParser);

    return 0;
}

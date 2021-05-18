#ifndef DUZE_ZAD_GAME_STATE_H
#define DUZE_ZAD_GAME_STATE_H

#include <cstdint>
#include <vector>
#include "../common/messages.h"
#include "RandomNumberGenerator.h"
#include "server_connection.h"
#include "arguments_parser_server.h"

enum Direction : uint_fast8_t {
    STRAIGHT = 0,
    RIGHT = 1,
    LEFT = 2,
};

struct Pixel {
    size_t x;
    size_t y;

    Pixel(double x, double y)
            : x(static_cast<size_t>(x)), y(static_cast<size_t>(y)) {}

    bool operator==(Pixel const &other) const {
        return x == other.x && y == other.y;
    }
};

struct Position {
    double x;
    double y;
    Pixel lastPixel;
    uint_fast32_t directionDegree;
    Direction direction = STRAIGHT; // todo

    Position(double x, double y, uint_fast32_t directionDegree)
            : x(x), y(y), lastPixel(x, y), directionDegree(directionDegree) {}

    // Returns true if pixel has been changed, false otherwise.
    bool move();
};

struct Client {
    uint_fast8_t port;
    uint_fast64_t session_id;
};

class GameState {
private:
    ServerToClientConnection const &clientConnection;
    uint_fast32_t const maxx;
    uint_fast32_t const maxy;
    uint_fast16_t const turningSpeed;
    uint_fast32_t const roundsPerSecond;
    uint_fast32_t game_id;
    std::vector<Position> players_positions;
//    std::vector<Event> events_history;
    std::vector<std::pair<void const *, size_t>> events_history;
    std::vector<std::vector<bool>> eaten;
    std::vector<Client> clients;
    RandomNumberGenerator randomNumberGenerator;

    void generateNewGame();

    void generatePixel(uint32_t x, uint32_t y);

    void generatePlayerEliminated(uint8_t playerNumber);

    void generateGameOver();

    void checkNewPosition(size_t index);

public:
    explicit GameState(ServerToClientConnection const &clientConnection,
                       ArgumentsParserServer const &argumentParser)
            : clientConnection(clientConnection),
              maxx(argumentParser.getWidth()), maxy(argumentParser.getHeight()),
              turningSpeed(argumentParser.getTurningSpeed()),
              roundsPerSecond(argumentParser.getRoundsPerSecond()),
              eaten(argumentParser.getWidth(),
                    std::vector<bool>(argumentParser.getHeight(), false)),
              randomNumberGenerator(argumentParser.getSeed()) {
        // TODO
        clients.push_back(Client());
        clients.push_back(Client());
    }

    ~GameState();

    void startGame();

    void round();

    void gameOver();
};

#endif //DUZE_ZAD_GAME_STATE_H

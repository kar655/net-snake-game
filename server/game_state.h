#ifndef DUZE_ZAD_GAME_STATE_H
#define DUZE_ZAD_GAME_STATE_H

#include <cstdint>
#include <vector>
#include <chrono>
#include <thread>

#include "../common/messages.h"
#include "RandomNumberGenerator.h"
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
    uint32_t directionDegree;
    Direction direction = STRAIGHT; // todo

    Position(double x, double y, uint32_t directionDegree)
            : x(x), y(y), lastPixel(x, y), directionDegree(directionDegree) {}

    // Returns true if pixel has been changed, false otherwise.
    bool move();
};

struct Client {
    uint_fast8_t port;
    uint_fast64_t session_id;
};

class GameState {
public:
    struct Event {
        void const *pointer;
        size_t size;
        EventsTypes type;

        Event(void const *pointer, size_t size, EventsTypes type)
                : pointer(pointer), size(size), type(type) {}
    };

    using EventHistory = std::vector<Event>;
private:
    uint_fast32_t const maxx;
    uint_fast32_t const maxy;
    uint_fast16_t const turningSpeed;
    uint_fast32_t const roundsPerSecond;
    std::chrono::milliseconds const timeBetweenRounds;
    uint_fast32_t game_id;
    std::vector<Position> players_positions;
//    std::vector<Event> events_history;
    EventHistory events_history;
    std::vector<std::vector<bool>> eaten;
    std::vector<Client> clients;
    RandomNumberGenerator randomNumberGenerator;
    bool hasEnded = false;

    void generateNewGame();

    void generatePixel(uint32_t x, uint32_t y);

    void generatePlayerEliminated(uint8_t playerNumber);

    void generateGameOver();

    void checkNewPosition(size_t index);

public:
    explicit GameState(ArgumentsParserServer const &argumentParser)
            : maxx(argumentParser.getWidth()), maxy(argumentParser.getHeight()),
              turningSpeed(argumentParser.getTurningSpeed()),
              roundsPerSecond(argumentParser.getRoundsPerSecond()),
              timeBetweenRounds(
                      static_cast<int>(1000.0 / static_cast<double>(argumentParser.getRoundsPerSecond()))),
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

    void roundsForSecond();

    void gameOver();

    [[nodiscard]] EventHistory const &getEvents() const {
        return events_history;
    }

    [[nodiscard]] uint32_t getGameId() const {
        return game_id;
    }

    [[nodiscard]] uint32_t getNewestEventIndex() const {
        return events_history.size();
    }

    [[nodiscard]] bool hasGameEnded() const {
        return hasEnded;
    }
};

#endif //DUZE_ZAD_GAME_STATE_H

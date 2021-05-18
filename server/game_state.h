#ifndef DUZE_ZAD_GAME_STATE_H
#define DUZE_ZAD_GAME_STATE_H

#include <cstdint>
#include <vector>
#include "../common/messages.h"
#include "RandomNumberGenerator.h"

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
    uint_fast32_t const maxx;
    uint_fast32_t const maxy;
    uint_fast16_t const turningSpeed;
    uint_fast32_t game_id;
    std::vector<Position> players_positions;
//    std::vector<Event> events_history;
    std::vector<std::vector<bool>> eaten;
    std::vector<Client> clients;
    RandomNumberGenerator randomNumberGenerator;
public:
    void startGame();

    void round();

    void gameOver();
};

#endif //DUZE_ZAD_GAME_STATE_H

#ifndef DUZE_ZAD_GAME_STATE_H
#define DUZE_ZAD_GAME_STATE_H

#include <cstdint>
#include <vector>
#include "events.h"

struct Position {
    double x;
    double y;
};

struct Client {
    uint_fast8_t port;
    uint_fast64_t session_id;
};

class GameState {
private:
    uint_fast32_t game_id;
    std::vector<Position> players_positions;
    std::vector<Event> events_history;
    std::vector<std::vector<bool>> eaten;
    std::vector<Client> clients;
public:

};

#endif //DUZE_ZAD_GAME_STATE_H

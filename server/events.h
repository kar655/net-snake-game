#ifndef DUZE_ZAD_EVENTS_H
#define DUZE_ZAD_EVENTS_H

#include <cstdint>
#include <iostream>
#include <vector>

enum EventsTypes {
    NEW_GAME = 0,
    PIXEL = 1,
    PLAYER_ELIMINATED = 2,
    GAME_OVER = 3,
};

struct EventData {
    uint_fast8_t event_type;

    explicit EventData(uint_fast8_t event_type)
            : event_type(event_type) {}

    virtual std::ostream &operator<<(std::ostream &os) = 0;
};

struct EventNewGame : public EventData {
    uint_fast32_t maxx;
    uint_fast32_t maxy;
    std::vector<unsigned char[20]> players_names;

    explicit EventNewGame(uint_fast32_t maxx, uint_fast32_t maxy,
                          std::vector<unsigned char[20]> players_names)
            : EventData(NEW_GAME), maxx(maxx), maxy(maxy),
              players_names(std::move(players_names)) {}
};

struct EventPixel : public EventData {
    uint_fast8_t player_number;
    uint_fast32_t x;
    uint_fast32_t y;

    explicit EventPixel(uint_fast8_t player_name, uint_fast32_t x, uint_fast32_t y)
            : EventData(PIXEL), player_number(player_name), x(x), y(y) {}
};

struct EventPlayerEliminated : public EventData {
    uint_fast8_t player_number;

    explicit EventPlayerEliminated(uint_fast8_t player_name)
            : EventData(PLAYER_ELIMINATED), player_number(player_name) {}
};

struct EventGameOver : public EventData {
    explicit EventGameOver()
            : EventData(GAME_OVER) {}
};

struct Event {
    uint_fast32_t len;
    uint_fast32_t event_no;
    std::vector<EventData> events;
    uint_fast32_t cec32;
};


#endif //DUZE_ZAD_EVENTS_H

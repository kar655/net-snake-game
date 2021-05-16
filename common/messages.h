#ifndef DUZE_ZAD_MESSAGES_H
#define DUZE_ZAD_MESSAGES_H

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

struct ClientMessage {
    uint_fast64_t session_id;
    uint_fast8_t turn_direction;
    uint_fast32_t next_expected_event_no;
//    std::string player_name;
    unsigned char player_name[20] = "GigaKox";
};

std::ostream &operator<<(std::ostream &os, ClientMessage const &clientMessage);

// ************************************************************************************

enum EventsTypes : uint_fast8_t {
    NEW_GAME = 0,
    PIXEL = 1,
    PLAYER_ELIMINATED = 2,
    GAME_OVER = 3,
};

struct EventData {
    uint_fast8_t event_type;

    explicit EventData(uint_fast8_t event_type)
            : event_type(event_type) {}

//    virtual std::ostream &operator<<(std::ostream &os) = 0;
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
    uint_fast8_t event_type;
//    std::vector<EventData> events;
//    void *event_data;
    uint_fast32_t cec32;
};

std::ostream &operator<<(std::ostream &os, Event const &event);

struct ServerMessage {
    uint_fast32_t game_id;
//    void *events;
};


#endif //DUZE_ZAD_MESSAGES_H

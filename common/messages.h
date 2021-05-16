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
    char player_name[20] = "GigaKox";
};

std::ostream &operator<<(std::ostream &os, ClientMessage const &clientMessage);

// ************************************************************************************

enum EventsTypes : uint_fast8_t {
    NEW_GAME = 0,
    PIXEL = 1,
    PLAYER_ELIMINATED = 2,
    GAME_OVER = 3,
};

struct Event {
    virtual ~Event() = default;
};

struct EventNewGame {
    uint32_t maxx;
    uint32_t maxy;
    unsigned char *players_names[20];
//    std::vector<unsigned char[20]> players_names;
};

struct EventPixel {
    uint8_t player_number;
    uint32_t x;
    uint32_t y;
};

struct EventPlayerEliminated {
//    uint8_t player_number;

    uint32_t len;
    uint32_t event_no;
    uint8_t event_type = PLAYER_ELIMINATED;
    uint8_t player_number;
    uint32_t cec32;
};

struct EventGameOver {

};

//struct Event {
//    uint32_t len;
//    uint32_t event_no;
//    uint8_t event_type;
////    std::vector<EventData> events;
//    void *event_data;
//    uint32_t cec32;
//};

std::ostream &operator<<(std::ostream &os, Event const &event);
std::ostream &operator<<(std::ostream &os, EventPlayerEliminated const &event);

struct ServerMessage {
    uint_fast32_t game_id;
//    void *events;
};


#endif //DUZE_ZAD_MESSAGES_H

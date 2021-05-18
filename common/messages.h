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

enum EventsTypes : uint8_t {
    NEW_GAME = 0,
    PIXEL = 1,
    PLAYER_ELIMINATED = 2,
    GAME_OVER = 3,
};

//struct Event {
//    virtual ~Event() = default;
//};

struct EventNewGame {
    uint32_t len;
    uint32_t event_no;
    uint8_t event_type = NEW_GAME;
    uint32_t maxx = 4;
    uint32_t maxy = 5;
//    char const *players_names[20] = {"First", "AndSecond"};
    char const players_names[20] = "First";

    EventNewGame() : len(sizeof(event_no) + sizeof(event_type)
    + sizeof(maxx) + sizeof(maxy) + sizeof(players_names)) {}
//    std::vector<unsigned char[20]> players_names;
};

struct EventPixel {
    uint32_t len;
    uint32_t event_no;
    uint8_t event_type = PIXEL;
    uint32_t x;
    uint32_t y;
    uint32_t cec32;
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
    uint32_t len;
    uint32_t event_no;
    uint8_t event_type = GAME_OVER;
    uint32_t cec32;
};

std::ostream &operator<<(std::ostream &os, EventNewGame const &event);
std::ostream &operator<<(std::ostream &os, EventPixel const &event);
std::ostream &operator<<(std::ostream &os, EventPlayerEliminated const &event);
std::ostream &operator<<(std::ostream &os, EventGameOver const &event);


#endif //DUZE_ZAD_MESSAGES_H

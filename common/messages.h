#ifndef DUZE_ZAD_MESSAGES_H
#define DUZE_ZAD_MESSAGES_H

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include "control_sum.h"

struct ClientMessage {
    uint64_t session_id;
    uint8_t turn_direction = 0;
    uint32_t next_expected_event_no = 0;
//    std::string player_name;
    char player_name[20] = "GigaKox";

    ClientMessage()
            : session_id(time(nullptr)) {}
};

std::ostream &operator<<(std::ostream &os, ClientMessage const &clientMessage);


enum EventsTypes : uint8_t {
    NEW_GAME = 0,
    PIXEL = 1,
    PLAYER_ELIMINATED = 2,
    GAME_OVER = 3,
};

// 40
struct EventNewGame {
    uint32_t len;
    uint32_t event_no;
    uint8_t event_type = NEW_GAME;
    uint32_t maxx = 4;
    uint32_t maxy = 5;
//    char const *players_names[20] = {"First", "AndSecond"};
    char const players_names[20] = "First";
    uint32_t crc32;

    EventNewGame(uint32_t event_no, uint32_t maxx, uint32_t maxy)
            : len(1), // TODO
              event_no(event_no), maxx(maxx), maxy(maxy) {
        crc32 = 0;
    }
//    std::vector<unsigned char[20]> players_names;
};

// 24
//struct __attribute__ ((packed)) EventPixel {
struct EventPixel {
    uint32_t len;
    uint32_t event_no;
    uint8_t event_type = PIXEL;
    uint32_t x;
    uint32_t y;
    uint32_t crc32;

    EventPixel(uint32_t event_no, uint32_t x, uint32_t y)
            : len(sizeof(event_no) + sizeof(event_type) + sizeof(x) + sizeof(y)),
              event_no(event_no), x(x), y(y) {
        crc32 = ControlSum::crc32Check(this, len + sizeof(len));
    }
};

// 16
struct EventPlayerEliminated {
//    uint8_t player_number;

    uint32_t len;
    uint32_t event_no;
    uint8_t event_type = PLAYER_ELIMINATED;
    uint8_t player_number;
    uint32_t crc32;

    EventPlayerEliminated(uint32_t event_no, uint8_t player_number)
            : len(sizeof(event_no) + sizeof(event_type) + sizeof(player_number)),
              event_no(event_no), player_number(player_number) {
        crc32 = ControlSum::crc32Check(this, len + sizeof(len));
    }
};

// 16
struct EventGameOver {
    uint32_t len;
    uint32_t event_no;
    uint8_t event_type = GAME_OVER;
    uint32_t crc32;

    EventGameOver(uint32_t event_no)
            : len(sizeof(event_no) + sizeof(event_type)),
              event_no(event_no) {
        crc32 = ControlSum::crc32Check(this, len + sizeof(len));
    }
};

std::ostream &operator<<(std::ostream &os, EventNewGame const &event);

std::ostream &operator<<(std::ostream &os, EventPixel const &event);

std::ostream &operator<<(std::ostream &os, EventPlayerEliminated const &event);

std::ostream &operator<<(std::ostream &os, EventGameOver const &event);


#endif //DUZE_ZAD_MESSAGES_H

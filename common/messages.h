#ifndef DUZE_ZAD_MESSAGES_H
#define DUZE_ZAD_MESSAGES_H

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <cstring>
#include "control_sum.h"

struct __attribute__ ((packed)) ClientMessage {
    uint64_t session_id = 0;
    uint8_t turn_direction = 0;
    uint32_t next_expected_event_no = 0;

    explicit ClientMessage()
            : session_id(time(nullptr)) {}
};

// Wraps pointer to ClientMessage with playerName
class ClientMessageWrapper {
private:
    void *clientMessageData;
    size_t const size;

public:
    explicit ClientMessageWrapper(std::string const &playerName);

    explicit ClientMessageWrapper(void *clientMessageData, size_t size);

    ~ClientMessageWrapper() {
        std::free(clientMessageData);
    }

    void setTurnDirection(uint8_t direction) {
        *(static_cast<uint8_t *>(clientMessageData) + sizeof(uint64_t)) = direction;
    }

    void setEventNumber(uint32_t eventNumber) {
        *reinterpret_cast<uint32_t *>(static_cast<uint8_t *>(clientMessageData) + sizeof(uint64_t) +
                                      sizeof(uint8_t)) = eventNumber;
    }

    [[nodiscard]] uint64_t getSessionId() const {
        return *static_cast<uint64_t *>(clientMessageData);
    };

    [[nodiscard]] uint8_t getTurnDirection() const {
        return *(static_cast<uint8_t *>(clientMessageData) + sizeof(uint64_t));
    };

    [[nodiscard]] uint32_t getEventNumber() const {
        return *reinterpret_cast<uint32_t *>(static_cast<uint8_t *>(clientMessageData)
                                             + sizeof(uint64_t) + sizeof(uint8_t));
    };


    [[nodiscard]] void const *getMessage() const {
        return clientMessageData;
    }

    [[nodiscard]] size_t getSize() const {
        return size;
    }

    [[nodiscard]] std::string getPlayerName() const {
        return std::string(static_cast<char *>(clientMessageData) + sizeof(ClientMessage),
                           size - sizeof(ClientMessage));
    }
};

std::ostream &operator<<(std::ostream &os, ClientMessage const &clientMessage);

std::ostream &operator<<(std::ostream &os, ClientMessageWrapper const &clientMessage);


enum EventsTypes : uint8_t {
    NEW_GAME = 0,
    PIXEL = 1,
    PLAYER_ELIMINATED = 2,
    GAME_OVER = 3,
};

// 40
struct __attribute__ ((packed)) EventNewGame {
    uint32_t len;
    uint32_t event_no;
    uint8_t event_type = NEW_GAME;
    uint32_t maxx;
    uint32_t maxy;

    EventNewGame(size_t lengthOfNames, uint32_t event_no, uint32_t maxx, uint32_t maxy)
            : len(sizeof(event_no) + sizeof(event_type) + sizeof(maxx)
                  + sizeof(maxy) + lengthOfNames),
              event_no(event_no), maxx(maxx), maxy(maxy) {}
};

// 24
//struct __attribute__ ((packed)) EventPixel {
struct __attribute__ ((packed)) EventPixel {
    uint32_t len;
    uint32_t event_no;
    uint8_t event_type = PIXEL;
    uint8_t player_number;
    uint32_t x;
    uint32_t y;
    uint32_t crc32;

    EventPixel(uint32_t event_no, uint8_t player_number, uint32_t x, uint32_t y)
            : len(sizeof(event_no) + sizeof(event_type) + sizeof(player_number) + sizeof(x) + sizeof(y)),
              event_no(event_no), player_number(player_number), x(x), y(y) {
        crc32 = ControlSum::crc32Check(this, len + sizeof(len));
    }
};

// 16
struct __attribute__ ((packed)) EventPlayerEliminated {
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
struct __attribute__ ((packed)) EventGameOver {
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

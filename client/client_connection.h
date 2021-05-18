#ifndef DUZE_ZAD_CLIENT_CONNECTION_H
#define DUZE_ZAD_CLIENT_CONNECTION_H

#include <string>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <netinet/in.h>
#include "../common/messages.h"


enum Direction : uint_fast8_t {
    STRAIGHT = 0,
    RIGHT = 1,
    LEFT = 2,
};


class ClientToServerConnection {
private:
    int usingSocket;
    struct addrinfo *address_result;
    static size_t constexpr BUFFER_SIZE = 2000;
    static size_t constexpr DGRAM_SIZE = 550;
    char buffer[BUFFER_SIZE];

    void sendMessage(std::string const &message);

    void parseEvents(void *message, size_t size);

public:
    explicit ClientToServerConnection(std::string const &gameServer,
                                      uint_fast16_t port);

    ~ClientToServerConnection();

    void sendClientMessage();

    void receiveServerMessage();

    void receiveEvent();
};


// TCP Connection
class ClientToGUIConnection {
private:
    int usingSocket;
    static size_t constexpr BUFFER_SIZE = 2000;
    char buffer[BUFFER_SIZE];
    Direction direction = STRAIGHT;
    ClientMessage &clientMessage;

    enum KeyEvents {
        LEFT_KEY_DOWN = 0,
        LEFT_KEY_UP = 1,
        RIGHT_KEY_DOWN = 2,
        RIGHT_KEY_UP = 3,
    };

    static std::unordered_map<std::string, KeyEvents> const guiMessages;

    void sendMessage(std::string const &message);

    void changeDirection(KeyEvents keyEvent);

public:
    explicit ClientToGUIConnection(std::string const &guiServer, uint_fast16_t port,
                                   ClientMessage &clientMessage);

    ~ClientToGUIConnection();

    [[nodiscard]] int getSocket() const {
        return usingSocket;
    }

    [[nodiscard]] Direction getDirection() const {
        return direction;
    }

    void startReading();

    void initialMessage(uint32_t maxx, uint32_t maxy,
                        std::vector<std::string> const &playerNames);

    void sendPixel(uint32_t x, uint32_t y, std::string const &playerName);

    void sendPlayerEliminated(std::string const &playerName);
};

#endif //DUZE_ZAD_CLIENT_CONNECTION_H

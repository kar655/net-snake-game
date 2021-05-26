#ifndef DUZE_ZAD_CLIENT_CONNECTION_H
#define DUZE_ZAD_CLIENT_CONNECTION_H

#include <string>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <netinet/in.h>
#include <thread>
#include "../common/messages.h"
#include "arguments_parser_client.h"


enum Direction : uint_fast8_t {
    STRAIGHT = 0,
    RIGHT = 1,
    LEFT = 2,
};

// TCP Connection
class ClientToGUIConnection {
private:
    int usingSocket;
    static size_t constexpr BUFFER_SIZE = 2000;
    char buffer[BUFFER_SIZE];
    Direction direction = STRAIGHT;
    ClientMessageWrapper &clientMessage;
    volatile bool running = true;
    std::thread thread;
    std::vector<std::string> playersNames;

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
    explicit ClientToGUIConnection(ArgumentsParserClient const &argumentsParser,
                                   ClientMessageWrapper &clientMessage);

    ~ClientToGUIConnection();

    void startReading();

    void initialMessage(uint32_t maxx, uint32_t maxy,
                        std::vector<std::string> &&playerNames);

    void sendPixel(uint8_t playerNumber, uint32_t x, uint32_t y);

    void sendPlayerEliminated(uint8_t playerNumber);

    [[nodiscard]] bool isConnected() const {
        return running;
    }
};

class ClientToServerConnection {
private:
    static size_t constexpr DGRAM_SIZE = 550;

    int usingSocket;
    struct addrinfo *address_result;
    bool gameEnded = false;
    uint32_t currentGameId = 1000; // TODO
    std::unordered_set<uint32_t> previousGameIds;

    void parseEvents(void *message, size_t size,
                     ClientToGUIConnection &guiConnection, ClientMessageWrapper &clientMessage);

public:
    explicit ClientToServerConnection(std::string const &gameServer,
                                      uint_fast16_t port);

    ~ClientToServerConnection();

    void sendClientMessage(ClientMessageWrapper const &clientMessage);

    void receiveEvent(ClientToGUIConnection &guiConnection, ClientMessageWrapper &clientMessage);

    [[nodiscard]] bool receivedGameOver() const {
        return gameEnded;
    }
};

#endif //DUZE_ZAD_CLIENT_CONNECTION_H

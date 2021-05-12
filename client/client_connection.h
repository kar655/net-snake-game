#ifndef DUZE_ZAD_CLIENT_CONNECTION_H
#define DUZE_ZAD_CLIENT_CONNECTION_H

#include <string>
#include <cstdint>
#include <unordered_map>
#include <vector>

class ClientToServerConnection {

};

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

    enum KeyEvents {
        LEFT_KEY_DOWN = 0,
        LEFT_KEY_UP = 1,
        RIGHT_KEY_DOWN = 2,
        RIGHT_KEY_UP = 3,
    };

    Direction direction = STRAIGHT;

    static std::unordered_map<std::string, KeyEvents> const guiMessages;

    void sendMessage(std::string const &message);

    void changeDirection(KeyEvents keyEvent);

public:
    explicit ClientToGUIConnection(std::string const &guiServer, uint_fast16_t port);

    ~ClientToGUIConnection();

    [[nodiscard]] int getSocket() const {
        return usingSocket;
    }

    [[nodiscard]] Direction getDirection() const {
        return direction;
    }

    void startReading();

    void initialMessage(uint_fast32_t maxx, uint_fast32_t maxy,
                        std::vector<std::string> const &playerNames);

    void sendPixel(uint_fast32_t x, uint_fast32_t y, std::string const &playerName);

    void sendPlayerEliminated(std::string const &playerName);
};

#endif //DUZE_ZAD_CLIENT_CONNECTION_H

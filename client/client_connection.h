#ifndef DUZE_ZAD_CLIENT_CONNECTION_H
#define DUZE_ZAD_CLIENT_CONNECTION_H

#include <string>
#include <cstdint>
#include <unordered_map>

class ClientToServerConnection {

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

    static std::unordered_map<std::string, KeyEvents> const guiMessages;

    void initialMessage();
public:
    explicit ClientToGUIConnection(std::string const &guiServer, uint_fast16_t port);
    ~ClientToGUIConnection();

    [[nodiscard]] int getSocket() const {
        return usingSocket;
    }

    void startReading();
};

#endif //DUZE_ZAD_CLIENT_CONNECTION_H

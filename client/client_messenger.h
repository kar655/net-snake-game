#ifndef DUZE_ZAD_CLIENT_MESSENGER_H
#define DUZE_ZAD_CLIENT_MESSENGER_H

#include "client_messages.h"
#include "client_connection.h"
#include <chrono>

class ClientMessenger {
private:
    static std::chrono::milliseconds constexpr DELAY{30};
    ClientMessage const &message;
    volatile bool running = true;

public:
    explicit ClientMessenger(ClientMessage const &message)
            : message(message) {}

    ~ClientMessenger() {
        stopRunning();
    }

    void run(ClientToServerConnection &serverConnection);

    void stopRunning() {
        running = false;
    }
};

#endif //DUZE_ZAD_CLIENT_MESSENGER_H

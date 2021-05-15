#ifndef DUZE_ZAD_CLIENT_MESSENGER_H
#define DUZE_ZAD_CLIENT_MESSENGER_H

#include "client_messages.h"
#include "client_connection.h"
#include <chrono>

class ClientMessenger {
private:
//    static uint_fast64_t constexpr MILLISECONDS = 30;
    static uint_fast64_t constexpr MILLISECONDS = 30;
    static std::chrono::milliseconds constexpr DELAY{MILLISECONDS};
    ClientMessage const &message;
    volatile bool running = true;

public:
    explicit ClientMessenger(ClientMessage const &message)
            : message(message) {}

    void run(ClientToServerConnection &serverConnection);

    void stopRunning() {
        running = false;
    }
};

#endif //DUZE_ZAD_CLIENT_MESSENGER_H

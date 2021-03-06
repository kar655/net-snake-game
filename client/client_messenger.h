#ifndef DUZE_ZAD_CLIENT_MESSENGER_H
#define DUZE_ZAD_CLIENT_MESSENGER_H

#include "../common/messages.h"
#include "client_connection.h"

#include <chrono>

class ClientMessenger {
private:
    static std::chrono::milliseconds constexpr DELAY{30};

    ClientMessageWrapper const &message;
    volatile bool running = true;

public:
    explicit ClientMessenger(ClientMessageWrapper const &clientMessage,
                             ClientToServerConnection &serverConnection);

    ~ClientMessenger() {
        running = false;
    }
};

#endif //DUZE_ZAD_CLIENT_MESSENGER_H

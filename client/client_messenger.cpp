#include "client_messenger.h"

#include <iostream>
#include <thread>

ClientMessenger::ClientMessenger(ClientMessageWrapper const &clientMessage,
                                 ClientToServerConnection &serverConnection)
                                 : message(clientMessage) {

    std::thread thread([this, &serverConnection]() -> void {
        auto wakeUp = std::chrono::steady_clock::now();

        while (running) {
            std::cout << message << std::endl;
            serverConnection.sendClientMessage(message);
            wakeUp += DELAY;
            std::this_thread::sleep_until(wakeUp);
        }
    });

    thread.detach();
}

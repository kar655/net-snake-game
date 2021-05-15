#include "client_messenger.h"

#include <iostream>
#include <thread>

void ClientMessenger::run(ClientToServerConnection &serverConnection) {

    std::thread thread([this, &serverConnection]() -> void {
        auto wakeUp = std::chrono::steady_clock::now();

        while (running) {
            std::cout << message << std::endl;
            wakeUp += DELAY;
            std::this_thread::sleep_until(wakeUp);
        }
    });

    thread.detach();
}

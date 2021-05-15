#include "client_messenger.h"

#include <iostream>
#include <thread>

void ClientMessenger::run(ClientToServerConnection &serverConnection) {

    std::thread thread([this, &serverConnection]() -> void {
        while (running) {
            std::cout << message << std::endl;
            std::this_thread::sleep_for(DELAY);
        }
    });

    thread.detach();
}

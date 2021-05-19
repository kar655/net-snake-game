#ifndef DUZE_ZAD_SERVER_CONNECTION_H
#define DUZE_ZAD_SERVER_CONNECTION_H

#include <string>
#include <cstdint>
#include <netinet/in.h>
#include <vector>
#include <thread>
#include "../common/messages.h"
#include "game_state.h"

class ServerToClientConnection {
private:
    GameState const &gameState;
    int usingSocket;
    struct sockaddr_in client_address;
    std::thread thread;
    bool hasSendGameOver = false;
    Direction &direction;


    void parseClientMessage(ClientMessage const &clientMessage);

    void sendEvent(void const *event, size_t eventLength);

public:
    explicit ServerToClientConnection(GameState const &gameState,
                                      uint_fast16_t port, Direction &direction);

    ~ServerToClientConnection();

    void receiveClientMessage();

    void sendEventsHistory(uint32_t gameId, size_t begin, size_t end);

    // Creates thread. Keeps listening to client message.
    // If new event occurred it will be sent.
    void run();
};

class ServerConnectionManager {
private:
    int usingSocket;
    struct sockaddr_in server;

    static int constexpr LISTEN_QUEUE = 5;

    static void handleNewClient(int newSocket);

public:
    explicit ServerConnectionManager(uint_fast16_t port);

    ~ServerConnectionManager();

    void waitForNewClient();

    void closeAllConnections();
};

#endif //DUZE_ZAD_SERVER_CONNECTION_H

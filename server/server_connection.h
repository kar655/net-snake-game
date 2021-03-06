#ifndef DUZE_ZAD_SERVER_CONNECTION_H
#define DUZE_ZAD_SERVER_CONNECTION_H

#include <unordered_map>
#include <string>
#include <cstdint>
#include <netinet/in.h>
#include <vector>
#include <thread>
#include <atomic>


#include "../common/messages.h"
#include "game_state.h"


class ServerToClientConnection;

class ClientHandler {
private:
    static size_t constexpr DGRAM_SIZE = 550;

    int const usingSocket;
    uint64_t const sessionId;
    struct sockaddr_in client_address;
    GameState &gameState;
    std::thread thread;
    bool hasSendGameOver = false;
    Direction &direction;
    std::atomic_size_t &gameOverSent;

    void sendEvent(void const *event, size_t eventLength);

    void sendEventsHistory(uint32_t gameId, size_t begin, size_t end);

public:

    explicit ClientHandler(int usingSocket, uint64_t sessionId,
                           struct sockaddr_in client_address,
                           GameState &gameState,
                           Direction &direction,
                           std::atomic_size_t &gameOverSent);

    ~ClientHandler();

    // Parse client message in new thread
    void parseClientMessage(ClientMessageWrapper const &clientMessage);

    friend class ServerToClientConnection;

    [[nodiscard]] uint64_t getSessionId() const {
        return sessionId;
    }
};

class ServerToClientConnection {
private:
    static size_t constexpr DGRAM_SIZE = 550;

    GameState &gameState;
    int usingSocket;
    std::thread thread;
    volatile bool running = true;
    std::atomic_size_t gameOverSent = 0;
    // port -> ClientHandler
    std::unordered_map<in_port_t, ClientHandler> clientHandlers;


    void receiveClientMessage();

    void handleClientMessage(struct sockaddr_in client_address,
                             ClientMessageWrapper const &message);

public:
    explicit ServerToClientConnection(GameState &gameState,
                                      uint_fast16_t port);

    ~ServerToClientConnection();

    // Creates thread. Keeps listening to client message.
    // If new event occurred it will be sent.
    void run();
};

#endif //DUZE_ZAD_SERVER_CONNECTION_H

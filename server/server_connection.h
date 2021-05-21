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
    GameState const &gameState;
    std::thread thread;
    bool hasSendGameOver = false;
    Direction &direction;
    std::atomic_size_t &gameOverSent;

    void sendEvent(void const *event, size_t eventLength);

    void sendEventsHistory(uint32_t gameId, size_t begin, size_t end);

public:

    explicit ClientHandler(int usingSocket, uint64_t sessionId,
                           struct sockaddr_in client_address,
                           GameState const &gameState,
                           Direction &direction,
                           std::atomic_size_t &gameOverSent);

    ~ClientHandler();

    // Parse client message in new thread
    void parseClientMessage(ClientMessage clientMessage);

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
//    struct sockaddr_in client_address;
    std::thread thread;
    volatile bool running = true;
//    bool hasSendGameOver = false;
    std::atomic_size_t gameOverSent = 0;
//    Direction &direction;

    // port -> ClientHandler
    std::unordered_map<in_port_t, ClientHandler> clientHandlers;

//    void parseClientMessage(ClientMessage const &clientMessage);
//
//    void sendEvent(void const *event, size_t eventLength);

    void receiveClientMessage();

    void handleClientMessage(struct sockaddr_in client_address, ClientMessage const &message);

public:
    explicit ServerToClientConnection(GameState &gameState,
                                      uint_fast16_t port);

    ~ServerToClientConnection();

//    void receiveClientMessage();

//    void sendEventsHistory(uint32_t gameId, size_t begin, size_t end);

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

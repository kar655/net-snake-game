#ifndef DUZE_ZAD_SERVER_CONNECTION_H
#define DUZE_ZAD_SERVER_CONNECTION_H

#include <string>
#include <cstdint>
#include <netinet/in.h>
#include <vector>
#include "../common/messages.h"

class ServerToClientConnection {
private:
    int usingSocket;
    struct sockaddr_in client_address;
    static size_t constexpr BUFFER_SIZE = 2000;
    char buffer[BUFFER_SIZE];

    void sendMessage(std::string const &message);

public:
    explicit ServerToClientConnection(uint_fast16_t port);

//    explicit ServerToClientConnection(int socket);

    ~ServerToClientConnection();

    void sendServerMessage(std::string const &message);

    void receiveClientMessage();

    void sendEvent(void const *event, size_t eventLength);

    // eventy musza byc robione z new i w detruktorze potem czyszczenie
    // alokacja na wszystko i na chama przypisywanie co gdzie jest
    void sendEventsHistory(uint32_t gameId, std::vector<std::pair<void const *, size_t>> const &events);
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

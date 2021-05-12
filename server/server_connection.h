#ifndef DUZE_ZAD_SERVER_CONNECTION_H
#define DUZE_ZAD_SERVER_CONNECTION_H

#include <string>
#include <cstdint>
#include <netinet/in.h>

class ServerToClientConnection {
private:
    int usingSocket;
    struct sockaddr_in client_address;
    static size_t constexpr BUFFER_SIZE = 2000;
    char buffer[BUFFER_SIZE];

    void sendMessage(std::string const &message);

public:
    explicit ServerToClientConnection(uint_fast16_t port);

    ~ServerToClientConnection();

    void sendServerMessage(std::string const &message);

    void receiveClientMessage();
};

#endif //DUZE_ZAD_SERVER_CONNECTION_H

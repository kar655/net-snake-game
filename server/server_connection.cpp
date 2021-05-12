#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "server_connection.h"

void ServerToClientConnection::sendMessage(std::string const &message) {

}

ServerToClientConnection::ServerToClientConnection(uint_fast16_t port) {
    memset(&buffer, 0, sizeof(buffer));
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    memset(&client_address, 0, sizeof(client_address));

    usingSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (usingSocket < 0) {
        std::cerr << "Can't open socket to client connection" << std::endl;
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    if (bind(usingSocket, reinterpret_cast<const sockaddr *>(&server_address),
             sizeof(server_address)) < 0) {
        std::cerr << "Can't bind socket to client connection" << std::endl;
        exit(1);
    }
}

ServerToClientConnection::~ServerToClientConnection() {
    close(usingSocket);
}

void ServerToClientConnection::sendServerMessage(std::string const &message) {
    socklen_t addressLength = sizeof(client_address);
    ssize_t sentLength = sendto(usingSocket, message.c_str(), message.length(), 0,
                                reinterpret_cast<const sockaddr *>(&client_address),
                                addressLength);

    if (sentLength != static_cast<ssize_t>(message.length())) {
        std::cerr << "Error sendto" << std::endl;
        exit(1);
    }
}

void ServerToClientConnection::receiveClientMessage() {
    socklen_t addressLength = sizeof(client_address);
    ssize_t receivedLength = recvfrom(usingSocket, buffer, sizeof(buffer), 0,
                           reinterpret_cast<sockaddr *>(&client_address),
                           &addressLength);

    if (receivedLength < 0) {
        std::cerr << "ERROR recvfrom" << std::endl;
        exit(1);
    }

    std::string message(buffer, receivedLength);
    std::cout << "Got message: '" << message << '\'' << std::endl;
}

#include "client_connection.h"

#include <cstdlib>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <iomanip>

std::unordered_map<std::string, ClientToGUIConnection::KeyEvents> const
        ClientToGUIConnection::guiMessages = {
        {"LEFT_KEY_DOWN\n",  LEFT_KEY_DOWN},
        {"LEFT_KEY_UP\n",    LEFT_KEY_UP},
        {"RIGHT_KEY_DOWN\n", RIGHT_KEY_DOWN},
        {"RIGHT_KEY_UP\n",   RIGHT_KEY_UP},
};

void ClientToGUIConnection::initialMessage() {
    static std::string const message = "NEW_GAME 50 100 player_name1 player_name2";

    if (write(usingSocket, message.c_str(), message.length())
        != static_cast<ssize_t>(message.length())) {
        std::cerr << "Write error" << std::endl;
    }
}

ClientToGUIConnection::ClientToGUIConnection(
        const std::string &guiServer,
        uint_fast16_t port) {
    memset(&buffer, 0, sizeof(buffer));
    std::string portStr = std::to_string(port);
    std::cout << "Starting connection: " << guiServer
              << " " << portStr << " " << port << std::endl;


    struct addrinfo address_hints;
    struct addrinfo *address_result;

    memset(&address_hints, 0, sizeof(address_hints));
    address_hints.ai_family = AF_INET6; // | AF_INET
    address_hints.ai_socktype = SOCK_STREAM;
    address_hints.ai_protocol = IPPROTO_TCP;

    int error = getaddrinfo(guiServer.c_str(), portStr.c_str(),
                            &address_hints, &address_result);
    if (error != 0) {
        std::cerr << "ERROR: " << gai_strerror(error)
                  << " getaddrinfo " << guiServer << " " << portStr << std::endl;
        exit(1);
    }

    usingSocket = socket(address_result->ai_family, address_result->ai_socktype,
                         address_result->ai_protocol);

    if (usingSocket < 0) {
        std::cerr << "Can't open socket to gui connection" << std::endl;
        exit(1);
    }

    error = connect(usingSocket, address_result->ai_addr, address_result->ai_addrlen);
    if (error < 0) {
        std::cerr << "Connect error" << std::endl;
        exit(1);
    }

    freeaddrinfo(address_result);

    initialMessage();
}

void ClientToGUIConnection::startReading() {
    int i = 0;
    while (++i < 300) {
        ssize_t receivedLength = read(usingSocket, buffer, BUFFER_SIZE - 1);
        if (receivedLength < 0) {
            std::cerr << "Read error" << std::endl;
            exit(1);
        }

        std::string message(buffer, receivedLength);
        std::cout << "Got message: '" << message << "'";

        auto iter = guiMessages.find(message);
        if (iter == guiMessages.end()) {
            std::cerr << "Unknown message!" << std::endl;
        }
        else {
            std::cout << "Action code is " << iter->second << std::endl;
        }
    }
}

ClientToGUIConnection::~ClientToGUIConnection() {
    close(usingSocket);
}

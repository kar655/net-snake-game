#include "client_connection.h"

#include <cstdlib>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <iomanip>


void ClientToServerConnection::sendMessage(std::string const &message) {
    ssize_t sendLength = sendto(usingSocket, message.c_str(), message.length(),
                                0, address_result->ai_addr, address_result->ai_addrlen);

    if (sendLength != static_cast<ssize_t>(message.length())) {
        std::cerr << "Write error" << std::endl;
        exit(1);
    }
}


ClientToServerConnection::ClientToServerConnection(std::string const &gameServer,
                                                   uint_fast16_t port) {
    memset(&buffer, 0, sizeof(buffer));
    std::string portStr = std::to_string(port);
    struct addrinfo address_hints;
    memset(&address_hints, 0, sizeof(address_hints));

    address_hints.ai_family = AF_INET;
    address_hints.ai_socktype = SOCK_DGRAM;
    address_hints.ai_protocol = IPPROTO_UDP;

    int error = getaddrinfo(gameServer.c_str(), portStr.c_str(),
                            &address_hints, &address_result);

    if (error != 0) {
        std::cerr << "ERROR: " << gai_strerror(error)
                  << " getaddrinfo " << gameServer << " " << portStr << std::endl;
        exit(1);
    }

    usingSocket = socket(address_result->ai_family, address_result->ai_socktype,
                         address_result->ai_protocol);

    if (usingSocket < 0) {
        std::cerr << "Can't open socket to game server connection" << std::endl;
        exit(1);
    }
}

ClientToServerConnection::~ClientToServerConnection() {
    freeaddrinfo(address_result);
    close(usingSocket);
}

void ClientToServerConnection::sendClientMessage() {
    std::string const message = "Hello World!\n";
    std::cout << "Starting sending messages" << std::endl;


    for (int i = 0; i < 1; ++i) {
        sendMessage(message);
    }
    std::cout << "Ending sending messages" << std::endl;
}

void ClientToServerConnection::receiveServerMessage() {
    ssize_t receivedLength = recvfrom(usingSocket, buffer, sizeof(buffer),
                                      0, address_result->ai_addr,
                                      &address_result->ai_addrlen);
    if (receivedLength < 0) {
        std::cerr << "ERROR recvfrom" << std::endl;
    }

    std::string message(buffer, receivedLength);

    std::cout << "Got message: '" << message << '\'' << std::endl;
}


std::unordered_map<std::string, ClientToGUIConnection::KeyEvents> const
        ClientToGUIConnection::guiMessages = {
        {"LEFT_KEY_DOWN\n",  LEFT_KEY_DOWN},
        {"LEFT_KEY_UP\n",    LEFT_KEY_UP},
        {"RIGHT_KEY_DOWN\n", RIGHT_KEY_DOWN},
        {"RIGHT_KEY_UP\n",   RIGHT_KEY_UP},
};

void ClientToGUIConnection::sendMessage(std::string const &message) {
    if (write(usingSocket, message.c_str(), message.length())
        != static_cast<ssize_t>(message.length())) {
        std::cerr << "Write error" << std::endl;
        exit(1);
    }
}

void ClientToGUIConnection::changeDirection(ClientToGUIConnection::KeyEvents keyEvent) {
    if (direction == LEFT) {
        if (keyEvent == LEFT_KEY_UP) {
            direction = STRAIGHT;
        }
        else if (keyEvent == RIGHT_KEY_DOWN) {
            direction = RIGHT;
        }
    }
    else if (direction == RIGHT) {
        if (keyEvent == RIGHT_KEY_UP) {
            direction = STRAIGHT;
        }
        else if (keyEvent == LEFT_KEY_DOWN) {
            direction = LEFT;
        }
    }
    else {
        if (keyEvent == LEFT_KEY_DOWN) {
            direction = LEFT;
        }
        else if (keyEvent == RIGHT_KEY_DOWN) {
            direction = RIGHT;
        }
    }
    clientMessage.turn_direction = direction;
}

ClientToGUIConnection::ClientToGUIConnection(
        const std::string &guiServer,
        uint_fast16_t port,
        ClientMessage &clientMessage)
        : clientMessage(clientMessage) {
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
}

ClientToGUIConnection::~ClientToGUIConnection() {
    close(usingSocket);
}

void ClientToGUIConnection::startReading() {
    int i = 0;
    while (++i < 200) {
        ssize_t receivedLength = read(usingSocket, buffer, BUFFER_SIZE - 1);
        if (receivedLength < 0) {
            std::cerr << "Read error" << std::endl;
            exit(1);
        }

        std::string message(buffer, receivedLength);

        auto iter = guiMessages.find(message);
        if (iter == guiMessages.end()) {
            // TODO prevent concatenating of sent messages
            //  maybe just check first x characters from message
            //  or use regex iterator.
            std::cerr << "Unknown message from GUI! Got: '"
                      << message << "'" << std::endl;
        }
        else {
//            std::cout << "Moving from " << direction << " to ";
            changeDirection(iter->second);
//            std::cout << direction << std::endl;
        }
    }
}

void ClientToGUIConnection::initialMessage(uint_fast32_t maxx, uint_fast32_t maxy,
                                           std::vector<std::string> const &playerNames) {
    std::string message = "NEW_GAME ";
    message += std::to_string(maxx);
    message += ' ';
    message += std::to_string(maxy);

    for (std::string const &playerName : playerNames) {
        message += ' ';
        message += playerName;
    }

    message += '\n';

    sendMessage(message);
}

void ClientToGUIConnection::sendPixel(uint_fast32_t x, uint_fast32_t y,
                                      std::string const &playerName) {
    std::string message = "PIXEL ";
    message += std::to_string(x);
    message += ' ';
    message += std::to_string(y);
    message += ' ';
    message += playerName;
    message += '\n';

    sendMessage(message);
}

void ClientToGUIConnection::sendPlayerEliminated(std::string const &playerName) {
    std::string message = "PLAYER_ELIMINATED ";
    message += playerName;
    message += '\n';

    sendMessage(message);
}

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

void ClientToServerConnection::receiveEvent() {
//    Event event;

    void *eventPointer = malloc(DGRAM_SIZE);
    if (eventPointer == nullptr) {
        std::cerr << "ERROR malloc" << std::endl;
    }

    // todo co jak sie zmienia rozmiar eventPointer????
    ssize_t receivedLength = recvfrom(usingSocket, eventPointer, DGRAM_SIZE,
                                      0, address_result->ai_addr,
                                      &address_result->ai_addrlen);
    if (receivedLength < 0) {
        std::cerr << "ERROR recvfrom " << receivedLength << std::endl;
    }

    std::cout << "receivedLength = " << receivedLength << std::endl;

    parseEvents(eventPointer, static_cast<size_t>(receivedLength));
}

void ClientToServerConnection::parseEvents(void *message, size_t size) {
    std::vector<std::pair<void *, size_t>> events;
    auto const gameId = *static_cast<uint32_t *>(message);
    std::cout << "gameId = " << gameId << std::endl;
    size_t skipped = 4;

    void *currentPointer = static_cast<uint32_t *>(message) + 1;

    while (skipped < size) {
        size_t shift;
        auto const eventType = static_cast<EventsTypes>(*(static_cast<uint8_t *>(currentPointer) + 8));

        if (eventType == PLAYER_ELIMINATED) {
            std::cout << "PLAYER_ELIMINATED" << std::endl;
            auto *event = static_cast<EventPlayerEliminated *>(currentPointer);
            std::cout << "Raw ptr: " << event << std::endl;
            std::cout << "Got eventPointer: '" << event->event_no << '\'' << std::endl;
            shift = sizeof(EventPlayerEliminated);
        }
        else if (eventType == PIXEL) {
            std::cout << "PIXEL" << std::endl;
            auto *event = static_cast<EventPixel *>(currentPointer);
            std::cout << "Raw ptr: " << event << std::endl;
            std::cout << "Got eventPointer: '" << event->event_no << '\'' << std::endl;
            shift = sizeof(EventPixel);
        }
        else if (eventType == NEW_GAME) {
            std::cout << "NEW_GAME" << std::endl;
            auto *event = static_cast<EventNewGame *>(currentPointer);
            std::cout << "Raw ptr: " << event << std::endl;
            std::cout << "Got eventPointer: '" << event->event_no << '\'' << std::endl;
            std::cout << event->players_names << std::endl;
            shift = sizeof(EventNewGame);
        }
        else if (eventType == GAME_OVER) {
            std::cout << "GAME_OVER" << std::endl;
            auto *event = static_cast<EventGameOver *>(currentPointer);
            std::cout << "Raw ptr: " << event << std::endl;
            std::cout << "Got eventPointer: '" << event->event_no << '\'' << std::endl;
            shift = sizeof(EventGameOver);
        }
        else {
            std::cerr << "Unknown eventType = " << eventType << std::endl;
            exit(1);
        }

        skipped += shift;
        currentPointer = static_cast<uint8_t *>(currentPointer) + shift;
    }
    free(message);
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

#include <cstdlib>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

#include "client_connection.h"


ClientToServerConnection::ClientToServerConnection(std::string const &gameServer,
                                                   uint_fast16_t port) {
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

void ClientToServerConnection::sendClientMessage(ClientMessageWrapper const &clientMessage) {
    ssize_t sendLength = sendto(usingSocket, clientMessage.getMessage(), clientMessage.getSize(),
                                0, address_result->ai_addr, address_result->ai_addrlen);

    if (sendLength != static_cast<ssize_t>(clientMessage.getSize())) {
        std::cerr << "Write error" << std::endl;
        exit(1);
    }
}

void ClientToServerConnection::receiveEvent(ClientToGUIConnection &guiConnection,
                                            ClientMessageWrapper &clientMessage) {
    void *eventPointer = malloc(DGRAM_SIZE);
    if (eventPointer == nullptr) {
        std::cerr << "ERROR malloc" << std::endl;
    }

    ssize_t receivedLength = recvfrom(usingSocket, eventPointer, DGRAM_SIZE,
                                      0, address_result->ai_addr,
                                      &address_result->ai_addrlen);
    if (receivedLength < 0) {
        std::cerr << "ERROR recvfrom " << receivedLength << std::endl;
    }

    parseEvents(eventPointer, static_cast<size_t>(receivedLength),
                guiConnection, clientMessage);
}

void ClientToServerConnection::parseEvents(void *message, size_t size,
                                           ClientToGUIConnection &guiConnection,
                                           ClientMessageWrapper &clientMessage) {
    std::vector<std::pair<void *, size_t>> events;
    auto const gameId = be32toh(*static_cast<uint32_t *>(message));
    size_t skipped = sizeof(uint32_t);

    void *currentPointer = static_cast<uint32_t *>(message) + 1;

    while (skipped < size) {
        size_t shift;
        auto const eventType = static_cast<EventsTypes>(*(static_cast<uint8_t *>(currentPointer) + 8));
        bool const shouldBeSkipped = previousGameIds.find(gameId) != previousGameIds.end()
                                     || (gameId != currentGameId && eventType != NEW_GAME);
        bool resetEventNumber = false;
        uint32_t nextEventNumber;

        if (eventType == PLAYER_ELIMINATED) {
            auto *event = static_cast<EventPlayerEliminated *>(currentPointer);
            uint32_t const crc32 = ControlSum::crc32Check(currentPointer, be32toh(event->len) + sizeof(event->len));

            event->fromBigEndian();
            shift = sizeof(EventPlayerEliminated);

            if (!shouldBeSkipped && event->event_no == clientMessage.getEventNumber()) {
                if (crc32 != event->crc32) {
                    std::cerr << "INCORRECT crc32 CHECKSUM!" << std::endl;
                    break;
                }

                guiConnection.sendPlayerEliminated(event->player_number);
                nextEventNumber = event->event_no;
            }
        }
        else if (eventType == PIXEL) {
            auto *event = static_cast<EventPixel *>(currentPointer);
            uint32_t const crc32 = ControlSum::crc32Check(currentPointer, be32toh(event->len) + sizeof(event->len));

            event->fromBigEndian();
            shift = sizeof(EventPixel);

            if (!shouldBeSkipped && event->event_no == clientMessage.getEventNumber()) {
                if (crc32 != event->crc32) {
                    std::cerr << "INCORRECT crc32 CHECKSUM!" << std::endl;
                    break;
                }

                guiConnection.sendPixel(event->player_number, event->x, event->y);
                nextEventNumber = event->event_no;
            }
        }
        else if (eventType == NEW_GAME) {
            auto *event = static_cast<EventNewGame *>(currentPointer);
            uint32_t const crc32 = ControlSum::crc32Check(currentPointer, be32toh(event->len) + sizeof(event->len));

            event->fromBigEndian();

            uint32_t const eventCrc32 = be32toh(*reinterpret_cast<uint32_t *>(
                    static_cast<uint8_t *>(currentPointer) + event->len + sizeof(event->len)));

            if (crc32 != eventCrc32 && event->event_no == clientMessage.getEventNumber()) {
                std::cerr << "INCORRECT crc32 CHECKSUM!" << std::endl;
                break;
            }

            gameEnded = false;
            currentGameId = gameId;
            uint32_t const namesLength = event->len
                                         - sizeof(event->event_no)
                                         - sizeof(event->event_type)
                                         - sizeof(event->maxx)
                                         - sizeof(event->maxy);

            shift = event->len + sizeof(event->len) + sizeof(uint32_t); // uint32_t for crc32

            std::vector<std::string> playerNames;
            std::string currentName;

            char *playerNamePointer = static_cast<char *>(currentPointer) + sizeof(EventNewGame);

            for (uint32_t i = 0; i < namesLength; i++) {
                if (*(playerNamePointer + i) == '\0') {
                    playerNames.emplace_back(std::move(currentName));
                    currentName.clear();
                }
                else {
                    currentName += *(playerNamePointer + i);
                }
            }

            guiConnection.initialMessage(event->maxx, event->maxy, std::move(playerNames));
            nextEventNumber = event->event_no;
        }
        else if (eventType == GAME_OVER) {
            auto *event = static_cast<EventGameOver *>(currentPointer);
            uint32_t const crc32 = ControlSum::crc32Check(currentPointer, be32toh(event->len) + sizeof(event->len));

            event->fromBigEndian();
            shift = sizeof(EventGameOver);

            if (!shouldBeSkipped && event->event_no == clientMessage.getEventNumber()) {
                if (crc32 != event->crc32) {
                    std::cerr << "INCORRECT crc32 CHECKSUM!" << std::endl;
                    break;
                }

                resetEventNumber = true;
                previousGameIds.insert(currentGameId);
                currentGameId = 175; // to prevent getting old messages
                gameEnded = true;
            }
        }
        else {
            std::cerr << "Unknown eventType = " << eventType << std::endl;
            exit(1);
        }

        if (resetEventNumber) {
            clientMessage.setEventNumber(0);
        }
        else if (!shouldBeSkipped) {
            clientMessage.setEventNumber(nextEventNumber + 1);
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

    clientMessage.setTurnDirection(direction);
}

ClientToGUIConnection::ClientToGUIConnection(
        ArgumentsParserClient const &argumentsParser,
        ClientMessageWrapper &clientMessage)
        : clientMessage(clientMessage) {

    uint16_t const port = argumentsParser.getGuiPort();
    std::string const guiServer = argumentsParser.getGuiServer();

    memset(&buffer, 0, sizeof(buffer));
    std::string portStr = std::to_string(port);

    struct addrinfo address_hints;
    struct addrinfo *address_result;

    memset(&address_hints, 0, sizeof(address_hints));
    address_hints.ai_family = AF_INET; // | AF_INET6
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
    running = false;
    thread.join();
    close(usingSocket);
}

void ClientToGUIConnection::startReading() {
    thread = std::thread([this]() {
        char innerBuffer[BUFFER_SIZE];

        while (running) {
            ssize_t receivedLength = read(usingSocket, innerBuffer, BUFFER_SIZE - 1);
            if (receivedLength < 0) {
                std::cerr << "Read error" << std::endl;
                exit(1);
            }
            else if (receivedLength == 0) {
                std::cerr << "Closed GUI connection" << std::endl;
                close(usingSocket);
                exit(0);
            }

            std::string message(innerBuffer, receivedLength);

            auto iter = guiMessages.find(message);
            if (iter != guiMessages.end()) {
                changeDirection(iter->second);
            }
        }
    });
}

void ClientToGUIConnection::initialMessage(uint32_t maxx, uint32_t maxy,
                                           std::vector<std::string> &&playerNames) {
    playersNames = std::move(playerNames);
    std::string message = "NEW_GAME ";
    message += std::to_string(maxx);
    message += ' ';
    message += std::to_string(maxy);

    for (std::string const &player : playersNames) {
        message += ' ';
        message += player;
    }

    message += '\n';

    sendMessage(message);
}

void ClientToGUIConnection::sendPixel(uint8_t playerNumber, uint32_t x, uint32_t y) {
    std::string message = "PIXEL ";
    message += std::to_string(x);
    message += ' ';
    message += std::to_string(y);
    message += ' ';
    message += playersNames[playerNumber];
    message += '\n';

    sendMessage(message);
}

void ClientToGUIConnection::sendPlayerEliminated(uint8_t playerNumber) {
    std::string message = "PLAYER_ELIMINATED ";
    message += playersNames[playerNumber];
    message += '\n';

    sendMessage(message);
}

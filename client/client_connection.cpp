#include "client_connection.h"

#include <cstdlib>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>


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
//    running = false;
    freeaddrinfo(address_result);
    close(usingSocket);
}

void ClientToServerConnection::sendClientMessage(ClientMessage const &clientMessage) {
    ssize_t sendLength = sendto(usingSocket, &clientMessage, sizeof(clientMessage),
                                0, address_result->ai_addr, address_result->ai_addrlen);

    if (sendLength != static_cast<ssize_t>(sizeof(clientMessage))) {
        std::cerr << "Write error" << std::endl;
        exit(1);
    }
}

void ClientToServerConnection::receiveEvent(ClientToGUIConnection &guiConnection,
                                            ClientMessage &clientMessage) {
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
                                           ClientMessage &clientMessage) {
    std::vector<std::pair<void *, size_t>> events;
    auto const gameId = *static_cast<uint32_t *>(message);
    std::cout << "gameId = " << gameId << std::endl;
    size_t skipped = 4;

    void *currentPointer = static_cast<uint32_t *>(message) + 1;

    while (skipped < size) {
        size_t shift;
        auto const eventType = static_cast<EventsTypes>(*(static_cast<uint8_t *>(currentPointer) + 8));

        uint32_t nextEventNumber;

        if (eventType == PLAYER_ELIMINATED) {
            auto *event = static_cast<EventPlayerEliminated *>(currentPointer);
            std::cout << *event << std::endl;
            shift = sizeof(EventPlayerEliminated);

            guiConnection.sendPlayerEliminated(event->player_number);
            nextEventNumber = event->event_no;
        }
        else if (eventType == PIXEL) {
            auto *event = static_cast<EventPixel *>(currentPointer);
            std::cout << *event << std::endl;
            shift = sizeof(EventPixel);

            guiConnection.sendPixel(event->player_number, event->x, event->y);
            nextEventNumber = event->event_no;
        }
        else if (eventType == NEW_GAME) {
            auto *event = static_cast<EventNewGame *>(currentPointer);
            std::cout << *event << std::endl;

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


            uint32_t const crc32 = *reinterpret_cast<uint32_t *>(static_cast<uint8_t *>(currentPointer) + event->len +
                                                           sizeof(event->len));
            std::cout << "************************ GOT CRC32 " << crc32 << std::endl;

            guiConnection.initialMessage(event->maxx, event->maxy, std::move(playerNames));
            nextEventNumber = event->event_no;
        }
        else if (eventType == GAME_OVER) {
            auto *event = static_cast<EventGameOver *>(currentPointer);
            std::cout << *event << std::endl;
            shift = sizeof(EventGameOver);

            nextEventNumber = event->event_no;
            gameEnded = true;
        }
        else {
            std::cerr << "Unknown eventType = " << eventType << std::endl;
            exit(1);
        }

        clientMessage.next_expected_event_no = nextEventNumber + 1;
        skipped += shift;
        currentPointer = static_cast<uint8_t *>(currentPointer) + shift;
    }

    free(message);
}

//void ClientToServerConnection::run(ClientToGUIConnection &guiConnection, ClientMessage &clientMessage) {
//    std::thread thread([&]() -> void {
//        while (running) {
//            receiveEvent(guiConnection, clientMessage);
//        }
//    });
//
//    thread.detach();
//}


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
        ArgumentsParserClient const &argumentsParser,
        ClientMessage &clientMessage)
        : clientMessage(clientMessage) {

    uint16_t const port = argumentsParser.getGuiPort();
    std::string const guiServer = argumentsParser.getGuiServer();

    memset(&buffer, 0, sizeof(buffer));
    std::string portStr = std::to_string(port);
    std::cout << "Starting connection: " << guiServer
              << " " << portStr << " " << port << std::endl;

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
                std::cerr << "Empty message. Ending connection" << std::endl;
                break;
            }

            std::string message(innerBuffer, receivedLength);

            auto iter = guiMessages.find(message);
            if (iter == guiMessages.end()) {
                // TODO prevent concatenating of sent messages
                //  maybe just check first x characters from message
                //  or use regex iterator.
                std::cerr << "Unknown message from GUI! Got: '"
                          << message << "'" << std::endl;
            }
            else {
//                std::cout << "Moving from " << direction << " to ";
                changeDirection(iter->second);
//                std::cout << direction << std::endl;
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

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>
#include "server_connection.h"

void ServerToClientConnection::sendMessage(std::string const &message) {
    (void) message;
}

ServerToClientConnection::ServerToClientConnection(GameState const &gameState, uint_fast16_t port)
        : gameState(gameState) {
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
    running = false;
    thread.detach(); // todo or join?
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
    auto message = new ClientMessage();
    ssize_t receivedLength = recvfrom(usingSocket, message, sizeof(ClientMessage), 0,
                                      reinterpret_cast<sockaddr *>(&client_address),
                                      &addressLength);

    if (receivedLength < 0) {
        std::cerr << "ERROR recvfrom" << std::endl;
        exit(1);
    }

    parseClientMessage(*message);
    delete message;
}

void ServerToClientConnection::sendEvent(void const *event, size_t eventLength) {
    std::cout << "Sending event of length " << eventLength << ": " << event << std::endl;
    socklen_t addressLength = sizeof(client_address);
    ssize_t sentLength = sendto(usingSocket, event, eventLength, 0,
                                reinterpret_cast<const sockaddr *>(&client_address),
                                addressLength);

    if (sentLength != static_cast<ssize_t>(eventLength)) {
        std::cerr << "Error sendto" << std::endl;
        exit(1);
    }
}

void ServerToClientConnection::sendEventsHistory(
        uint32_t gameId,
        size_t begin, size_t end) {
    std::cout << "=========SENDING EVENTS FROM " << begin << " TO " << end << " OUT OF " << gameState.getNewestEventIndex() << std::endl;
    size_t sizeSum = sizeof(gameId);
    GameState::EventHistory const &eventHistory = gameState.getEvents();

    for (size_t i = begin; i < end; i++) {
        sizeSum += eventHistory[i].second;
    }

    void *serverMessage = malloc(sizeSum);
    if (serverMessage == nullptr) {
        std::cerr << "ERROR malloc" << std::endl;
        exit(1);
    }

    *static_cast<uint32_t *>(serverMessage) = gameId;
//    serverMessage->game_id = gameId;

    void *currentPointer = reinterpret_cast<uint32_t *>(serverMessage) + 1;

    for (size_t i = begin; i < end; i++) {
        std::memcpy(currentPointer, eventHistory[i].first, eventHistory[i].second);
        currentPointer = static_cast<uint8_t *>(currentPointer) + eventHistory[i].second;
    }

    sendEvent(serverMessage, sizeSum);
    free(serverMessage);
}

void ServerToClientConnection::run() {
    thread = std::thread([this]() -> void {
        while (running) {
            receiveClientMessage(); // TODO add poll here, because can lock when no message is sent
        }
    });
}

void ServerToClientConnection::parseClientMessage(ClientMessage const &clientMessage) {
    std::cout << "Got message: " << clientMessage << std::endl;
    uint32_t const lastEventId = gameState.getNewestEventIndex();

    sendEventsHistory(gameState.getGameId(), clientMessage.next_expected_event_no, lastEventId);
}

//ServerToClientConnection::ServerToClientConnection(int socket) {
//    usingSocket = socket;
//    std::cout << "SECOND CONSTRUCTOR" << std::endl;
//}


ServerConnectionManager::ServerConnectionManager(uint_fast16_t port) {
    memset(&server, 0, sizeof(server));

    usingSocket = socket(PF_INET, SOCK_STREAM, 0); // todo SOCK_DGRAM?

    if (usingSocket < 0) {
        std::cerr << "Can't open socket" << std::endl;
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    if (bind(usingSocket, reinterpret_cast<const sockaddr *>(&server),
             sizeof(server)) < 0) {
        std::cerr << "Can't bind socket to client connection" << std::endl;
        exit(1);
    }

    socklen_t addressLength = sizeof(server);
    if (getsockname(usingSocket, reinterpret_cast<sockaddr *>(&server), &addressLength) == -1) {
        std::cerr << "getsockname error" << std::endl;
        exit(1);
    }

    std::cout << "Listening at port " << ntohs(server.sin_port) << std::endl;

    if (listen(usingSocket, LISTEN_QUEUE) < 0) {
        std::cerr << "Listen error" << std::endl;
        exit(1);
    }
}

ServerConnectionManager::~ServerConnectionManager() {
    closeAllConnections();
}


void ServerConnectionManager::waitForNewClient() {
    int newSocket = accept(usingSocket, nullptr, nullptr);
    if (newSocket == -1) {
        std::cerr << "Accept error" << std::endl;
        exit(1);
    }

    // Handle connection in new thread.
    std::thread thread(handleNewClient, newSocket);
    thread.join(); // todo detach
}

void ServerConnectionManager::closeAllConnections() {
    std::cout << "In closeAllConnections" << std::endl;
}

void ServerConnectionManager::handleNewClient(int newSocket) {
    std::cout << "Hello from new thread at socket = " << newSocket << std::endl;
}

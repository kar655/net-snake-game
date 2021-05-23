#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>
#include "server_connection.h"

ClientHandler::ClientHandler(int usingSocket, uint64_t sessionId, struct sockaddr_in client_address,
                             GameState const &gameState,
                             Direction &direction,
                             std::atomic_size_t &gameOverSent)
        : usingSocket(usingSocket), sessionId(sessionId),
          client_address(client_address),
          gameState(gameState),
          direction(direction),
          gameOverSent(gameOverSent) {
    thread = std::thread([]() {});
}

ClientHandler::~ClientHandler() {
    thread.join();
}


void ClientHandler::sendEvent(void const *event, size_t eventLength) {
    socklen_t addressLength = sizeof(client_address);
    ssize_t sentLength = sendto(usingSocket, event, eventLength, 0,
                                reinterpret_cast<const sockaddr *>(&client_address),
                                addressLength);

    if (sentLength != static_cast<ssize_t>(eventLength)) {
        std::cerr << "Error sendto" << std::endl;
        exit(1);
    }
}

void ClientHandler::sendEventsHistory(uint32_t gameId, size_t begin, size_t end) {
    if (begin == end) {
//        std::cout << "No new events" << std::endl;
        return;
    }

//    std::cout << "SENDING EVENTS FROM " << begin << " TO " << end << " OUT OF "
//              << gameState.getNewestEventIndex() << std::endl;
    size_t sizeSum = sizeof(gameId);
    GameState::EventHistory const &eventHistory = gameState.getEvents();

    size_t exactEnd = end;

    for (size_t i = begin; i < end; i++) {
        if (eventHistory[i].size + sizeSum > DGRAM_SIZE) {
            exactEnd = i;
            std::cout << "LIMIT SET TO " << exactEnd << std::endl;
            break;
        }

        sizeSum += eventHistory[i].size;
    }

    void *serverMessage = malloc(sizeSum);
    if (serverMessage == nullptr) {
        std::cerr << "ERROR malloc" << std::endl;
        exit(1);
    }

    *static_cast<uint32_t *>(serverMessage) = gameId;

    void *currentPointer = reinterpret_cast<uint32_t *>(serverMessage) + 1;

    for (size_t i = begin; i < exactEnd; i++) {
        std::memcpy(currentPointer, eventHistory[i].pointer, eventHistory[i].size);
        currentPointer = static_cast<uint8_t *>(currentPointer) + eventHistory[i].size;

        if (eventHistory[i].type == GAME_OVER) {
            hasSendGameOver = true;
            gameOverSent++;
        }
    }

    sendEvent(serverMessage, sizeSum);
    free(serverMessage);
}


void ClientHandler::parseClientMessage(ClientMessage clientMessage) {
    thread.join();

    thread = std::thread([this, &clientMessage]() -> void {
        uint32_t const lastEventId = gameState.getNewestEventIndex();

        // set turn direction
        direction = static_cast<Direction>(clientMessage.turn_direction);
//        std::cerr << "******************* SETTING PLAYER " << clientMessage.player_name
//                  << " DIRECTION TO " << static_cast<int>(clientMessage.turn_direction) << std::endl;
        sendEventsHistory(gameState.getGameId(), clientMessage.next_expected_event_no, lastEventId);
    });
}

ServerToClientConnection::ServerToClientConnection(GameState &gameState,
                                                   uint_fast16_t port)
        : gameState(gameState), clientHandlers() {

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

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
    thread.join();

    close(usingSocket);
}

void ServerToClientConnection::receiveClientMessage() {
    struct sockaddr_in client_address;
    memset(&client_address, 0, sizeof(client_address));

    socklen_t addressLength = sizeof(client_address);
    auto message = new ClientMessage();
    ssize_t receivedLength = recvfrom(usingSocket, message, sizeof(ClientMessage), 0,
                                      reinterpret_cast<sockaddr *>(&client_address),
                                      &addressLength);

    if (receivedLength < 0) {
        std::cerr << "ERROR recvfrom" << std::endl;
        exit(1);
    }

    handleClientMessage(client_address, *message);

    delete message;
}

void ServerToClientConnection::handleClientMessage(struct sockaddr_in client_address,
                                                   ClientMessage const &message) {
//    std::cout << "Got connection from " << client_address.sin_addr.s_addr
//              << " port " << client_address.sin_port << std::endl;

    auto iter = clientHandlers.find(client_address.sin_port);
    if (iter == clientHandlers.end() || iter->second.getSessionId() != message.session_id) {
        std::cout << "NEW CONNECTION OR NOT KNOWN SESSION. MAKING NEW HANDLER" << std::endl;

        if (iter != clientHandlers.end()) {
            clientHandlers.erase(iter);
        }

        iter = clientHandlers.emplace(
                        std::piecewise_construct,
                        std::forward_as_tuple(client_address.sin_port),
                        std::forward_as_tuple(usingSocket, message.session_id, client_address, gameState,
                                              gameState.addClient(client_address.sin_port, message.session_id,
                                                                  message.player_name),
                                              gameOverSent))
                .first;
    }

    iter->second.parseClientMessage(message);
}

void ServerToClientConnection::run() {
    thread = std::thread([this]() -> void {
        while (running || gameOverSent != clientHandlers.size()) {
            receiveClientMessage(); // TODO add poll here, because can lock when no message is sent
        }
    });
}

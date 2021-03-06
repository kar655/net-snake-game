#include <iostream>
#include <cmath>

#include "game_state.h"


bool Position::move() {
    double const directionRadians = static_cast<double>(directionDegree) * M_PI / 180.0;
    x += std::cos(directionRadians);
    y += std::sin(directionRadians);

    Pixel newPixel(x, y);
    bool const hasChanged = !(lastPixel == newPixel);
    lastPixel = newPixel;

    return hasChanged;
}


GameState::GameState(const ArgumentsParserServer &argumentParser)
        : maxx(argumentParser.getWidth()),
          maxy(argumentParser.getHeight()),
          turningSpeed(argumentParser.getTurningSpeed()),
          roundsPerSecond(
                  argumentParser.getRoundsPerSecond()),
          timeBetweenRounds(
                  static_cast<int>(1000.0 /
                                   static_cast<double>(argumentParser.getRoundsPerSecond()))),
          eaten(argumentParser.getWidth(),
                std::vector<bool>(argumentParser.getHeight(),
                                  false)),
          randomNumberGenerator(argumentParser.getSeed()) {
    clients.reserve(MAX_PLAYER_NUMBER);
}

GameState::~GameState() {
    for (auto const &event : events_history) {
        std::free(const_cast<void *>(event.pointer));
    }
}

void GameState::generateNewGame() {
    size_t namesLength = 0;
    std::string namesConcatenated;

    for (auto const &client : clients) {
        if (client.spectator) {
            ++spectators;
            continue;
        }

        namesLength += client.name.length() + 1; // 1 for \0 char
        namesConcatenated += client.name;
        namesConcatenated += '\0';
    }

    auto event = EventNewGame(namesLength, events_history.size(), maxx, maxy);
    size_t const totalLength = sizeof(EventNewGame) + namesLength + sizeof(uint32_t);

    void *eventNewGame = std::malloc(totalLength);

    memcpy(eventNewGame, &event, sizeof(EventNewGame));
    memcpy(static_cast<uint8_t *>(eventNewGame) + sizeof(EventNewGame), namesConcatenated.c_str(),
           namesConcatenated.size());

    *reinterpret_cast<uint32_t *>(
            static_cast<uint8_t *>(eventNewGame) + totalLength - sizeof(uint32_t)
    ) = htobe32(ControlSum::crc32Check(eventNewGame, totalLength - sizeof(uint32_t)));

    events_history.emplace_back(eventNewGame, totalLength, NEW_GAME);
    sendNewEvent(events_history.size() - 1);
}

void GameState::generatePixel(uint8_t playerNumber, uint32_t x, uint32_t y) {
    auto event = new EventPixel(events_history.size(), playerNumber, x, y);
    events_history.emplace_back(event, sizeof(EventPixel), PIXEL);
    sendNewEvent(events_history.size() - 1);
}

void GameState::generatePlayerEliminated(uint8_t playerNumber) {
    clients[playerNumber].setDead();
    alivePlayers--;

    auto event = new EventPlayerEliminated(events_history.size(), playerNumber);
    events_history.emplace_back(event, sizeof(EventPlayerEliminated), PLAYER_ELIMINATED);
    sendNewEvent(events_history.size() - 1);
}

void GameState::generateGameOver() {
    auto event = new EventGameOver(events_history.size());
    events_history.emplace_back(event, sizeof(EventGameOver), GAME_OVER);
    hasEnded = true;
    sendNewEvent(events_history.size() - 1);
}

void GameState::checkNewPosition(size_t index) {
    Pixel const &pixel = players_positions[index].lastPixel;

    if (players_positions[index].x < 0 || pixel.x >= maxx
        || players_positions[index].y < 0 || pixel.y >= maxy) {
        std::cout << "Fall from board" << std::endl;
        generatePlayerEliminated(index);
    }
    else if (eaten[pixel.x][pixel.y]) {
        std::cout << "Pixel Already eaten." << std::endl;
        generatePlayerEliminated(index);
    }
    else {
        eaten[pixel.x][pixel.y] = true;
        generatePixel(index, pixel.x, pixel.y);
    }
}

void GameState::startGame() {
    game_id = randomNumberGenerator.generate();
    generateNewGame();

    for (size_t i = 0; i < clients.size(); ++i) {
        // Prevent undefined order
        auto const first = randomNumberGenerator.generate();
        auto const second = randomNumberGenerator.generate();
        auto const third = randomNumberGenerator.generate();

        players_positions.emplace_back(
                static_cast<double>(first % maxx) + 0.5,
                static_cast<double>(second % maxy) + 0.5,
                third % 360
        );

        checkNewPosition(i);
    }

    alivePlayers = clients.size();
}

void GameState::round() {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].dead || clients[i].spectator) {
            continue;
        }

        if (clients[i].direction == RIGHT) {
            players_positions[i].directionDegree += turningSpeed;
        }
        else if (clients[i].direction == LEFT) {
            // Prevent directionDegree flip
            players_positions[i].directionDegree += 360 - turningSpeed;
        }

        players_positions[i].directionDegree %= 360;

        if (players_positions[i].move()) {
            checkNewPosition(i);
        }
    }
}

void GameState::sendNewEvent(size_t index) {
    auto const &event = events_history.at(index);
    uint32_t const gameId = game_id;

    size_t const totalSize = event.size + sizeof(uint32_t);
    auto eventPointer = std::malloc(totalSize);
    *static_cast<uint32_t *>(eventPointer) = htobe32(gameId);
    std::memcpy(static_cast<uint8_t *>(eventPointer) + sizeof(uint32_t),
                event.pointer, event.size);

    for (auto &client : clients) {
        client.sendEvent(eventPointer, totalSize);
    }

    std::free(eventPointer);
}

void GameState::roundsForSecond() {
    auto wakeUp = std::chrono::steady_clock::now();

    for (uint32_t r = 0; r < roundsPerSecond && alivePlayers - spectators > 1; ++r) {
        wakeUp += timeBetweenRounds;
        round();
        std::this_thread::sleep_until(wakeUp);
    }
}

void GameState::playGame() {
    while (alivePlayers - spectators > 1) {
        roundsForSecond();
    }
}


void GameState::gameOver() {
    generateGameOver();
    playersReady = 0;
    players_positions.clear();

    for (auto &client : clients) {
        client.reset();
    }

    eaten = std::vector<std::vector<bool>>(maxx, std::vector<bool>(maxy, false));
    hasEnded = false;
    events_history.clear();
}

void GameState::waitForClients() {
    while (playersReady < MIN_PLAYER_NUMBER) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << static_cast<int>(playersReady) << " out of " << static_cast<int>(MIN_PLAYER_NUMBER) << std::endl;
    }
}

void GameState::setPlayerReady(in_port_t port) {
    if (!clients[clientsMap[port].second].ready) {
        clients[clientsMap[port].second].ready = true;
        ++playersReady;
    }
}

[[nodiscard]] Direction &GameState::addClient(int usingSocket, struct sockaddr_in client_address, uint64_t sessionId,
                                              std::string playerName) {
    size_t index;

    auto iter = clientsMap.find(client_address.sin_port);
    if (iter == clientsMap.end()) {
        clientsMap[client_address.sin_port] = {sessionId, clients.size()};
        index = clients.size();
    }
    else {
        index = iter->second.second;
    }

    clients.emplace_back(usingSocket, client_address, sessionId, index, std::move(playerName));

    return clients.back().direction;
}

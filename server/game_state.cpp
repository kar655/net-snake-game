#include "game_state.h"

#include <iostream>
#include <cmath>


bool Position::move() {
    double const directionRadians = static_cast<double>(directionDegree) * M_PI / 180.0;
    x -= std::sin(directionRadians); // TODO
    y += std::cos(directionRadians);

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
//        delete event.pointer;
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
    std::cout << event << " namesLength=" << namesConcatenated.length() << std::endl;

    size_t const totalLength = sizeof(EventNewGame) + namesLength + sizeof(uint32_t);

    void *eventNewGame = std::malloc(totalLength);

    memcpy(eventNewGame, &event, sizeof(EventNewGame));
    memcpy(static_cast<uint8_t *>(eventNewGame) + sizeof(EventNewGame), namesConcatenated.c_str(),
           namesConcatenated.size());

    *reinterpret_cast<uint32_t *>(
            static_cast<uint8_t *>(eventNewGame) + totalLength - sizeof(uint32_t)
    ) = ControlSum::crc32Check(eventNewGame, totalLength - sizeof(uint32_t));

    std::cout << "ADDING NEW GAME TO EVENTS size(expects0)==" << events_history.size() << std::endl;
    events_history.emplace_back(eventNewGame, totalLength, NEW_GAME);
}

void GameState::generatePixel(uint8_t playerNumber, uint32_t x, uint32_t y) {
    auto event = new EventPixel(events_history.size(), playerNumber, x, y);
    events_history.emplace_back(event, sizeof(EventPixel), PIXEL);
}

void GameState::generatePlayerEliminated(uint8_t playerNumber) {
    clients[playerNumber].setDead();
    alivePlayers--;

    auto event = new EventPlayerEliminated(events_history.size(), playerNumber);
    std::cout << *event << std::endl;
    events_history.emplace_back(event, sizeof(EventPlayerEliminated), PLAYER_ELIMINATED);
}

void GameState::generateGameOver() {
    auto event = new EventGameOver(events_history.size());
    std::cout << *event << std::endl;
    events_history.emplace_back(event, sizeof(EventGameOver), GAME_OVER);
    hasEnded = true;
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
        players_positions.emplace_back(
                static_cast<double>(randomNumberGenerator.generate() % maxx) + 0.5,
                static_cast<double>(randomNumberGenerator.generate() % maxy) + 0.5,
                randomNumberGenerator.generate() % 360
        );

        checkNewPosition(i);
    }

    alivePlayers = clients.size(); // todo what if client joins here???
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
        else {
            std::cout << "Hasn't moved " << i << std::endl;
        }
    }
}

void GameState::roundsForSecond() {
    auto wakeUp = std::chrono::steady_clock::now();

    for (uint32_t r = 0; r < roundsPerSecond; ++r) {
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
    std::this_thread::sleep_for(std::chrono::seconds(3));
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

[[nodiscard]] Direction &GameState::addClient(in_port_t port, uint64_t sessionId,
                                              std::string playerName) {
    size_t index;

    auto iter = clientsMap.find(port);
    if (iter == clientsMap.end()) {
        clientsMap[port] = {sessionId, clients.size()};
        index = clients.size();
    }
    else {
        index = iter->second.second;
    }

    // TODO
    clients.emplace_back(port, sessionId, index, std::move(playerName));

    return clients.back().direction;
}

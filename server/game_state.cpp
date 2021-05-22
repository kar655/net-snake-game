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
    auto event = new EventNewGame(events_history.size(), maxx, maxy);
    std::cout << *event << std::endl;
    events_history.emplace_back(event, sizeof(EventNewGame), NEW_GAME);
}

void GameState::generatePixel(uint8_t playerNumber, uint32_t x, uint32_t y) {
    auto event = new EventPixel(events_history.size(), playerNumber, x, y);
    std::cout << *event << std::endl;
    events_history.emplace_back(event, sizeof(EventPixel), PIXEL);
}

void GameState::generatePlayerEliminated(uint8_t playerNumber) {
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
        std::cout << "Eating pixel." << std::endl;
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
}

void GameState::round() {
    for (size_t i = 0; i < clients.size(); ++i) {
//        std::cerr << "==============CLIENT " << i << " HAS DIRECTION = " << clients[i].direction << std::endl;
        if (clients[i].direction == RIGHT) {
            players_positions[i].directionDegree += turningSpeed;
        }
        else if (clients[i].direction == LEFT) {
            players_positions[i].directionDegree -= turningSpeed;
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

void GameState::gameOver() {
    generateGameOver();
}

[[nodiscard]] Direction &GameState::addClient(in_port_t port,
                                              uint64_t sessionId) {
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
    clients.emplace_back(port, sessionId, index);

    return clients.back().direction;
}

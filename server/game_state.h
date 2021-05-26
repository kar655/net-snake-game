#ifndef DUZE_ZAD_GAME_STATE_H
#define DUZE_ZAD_GAME_STATE_H

#include <cstdint>
#include <vector>
#include <chrono>
#include <thread>
#include <netinet/in.h>
#include <unordered_map>
#include <atomic>

#include "../common/messages.h"
#include "random_number_generator.h"
#include "arguments_parser_server.h"


enum Direction : uint8_t {
    STRAIGHT = 0,
    RIGHT = 1,
    LEFT = 2,
};


struct Pixel {
    size_t x;
    size_t y;

    Pixel(double x, double y)
            : x(static_cast<size_t>(x)), y(static_cast<size_t>(y)) {}

    bool operator==(Pixel const &other) const {
        return x == other.x && y == other.y;
    }
};


struct Position {
    double x;
    double y;
    Pixel lastPixel;
    uint32_t directionDegree;

    Position(double x, double y, uint32_t directionDegree)
            : x(x), y(y), lastPixel(x, y), directionDegree(directionDegree) {}

    // Returns true if pixel has been changed, false otherwise.
    bool move();
};


struct Client {
    int const usingSocket;
    struct sockaddr_in client_address;
    uint64_t session_id;
    size_t positionIndex;
    Direction direction;
    bool const spectator;
    std::string name;
    bool ready = false;
    bool dead = false;

//    using EventSender = std::function<void(void const *event, size_t eventLength)>;
//    EventSender sendEvent;

    Client(int usingSocket, struct sockaddr_in client_address, uint64_t sessionId,
           size_t positionIndex, std::string name)
            : usingSocket(usingSocket),
              client_address(client_address),
              session_id(sessionId),
              positionIndex(positionIndex),
              direction(STRAIGHT),
              spectator(name.empty()),
              name(std::move(name)) {}


    void setReady() {
        ready = true;
    }

    void setDead() {
        dead = true;
    }

    void reset() {
        ready = false;
        dead = false;
    }

    void sendEvent(void const *event, size_t eventLength) {
        socklen_t addressLength = sizeof(client_address);
        ssize_t sentLength = sendto(usingSocket, event, eventLength, 0,
                                    reinterpret_cast<const sockaddr *>(&client_address),
                                    addressLength);

        if (sentLength != static_cast<ssize_t>(eventLength)) {
            std::cerr << "Error sendto" << std::endl;
            exit(1);
        }
    }
};


class GameState {
public:
    struct Event {
        void const *pointer;
        size_t size;
        EventsTypes type;

        Event(void const *pointer, size_t size, EventsTypes type)
                : pointer(pointer), size(size), type(type) {}
    };

    using EventHistory = std::vector<Event>;
private:
    static uint8_t constexpr MAX_PLAYER_NUMBER = 100;
    static uint8_t constexpr MIN_PLAYER_NUMBER = 2;
    uint_fast32_t const maxx;
    uint_fast32_t const maxy;
    uint_fast16_t const turningSpeed;
    uint_fast32_t const roundsPerSecond;
    std::chrono::milliseconds const timeBetweenRounds;
    uint_fast32_t game_id;
    std::vector<Position> players_positions;
    EventHistory events_history;
    std::vector<std::vector<bool>> eaten;
    std::vector<Client> clients;
    RandomNumberGenerator randomNumberGenerator;
    bool hasEnded = false;
    uint32_t spectators = 0;

    std::atomic_uint8_t playersReady = 0;
    uint32_t alivePlayers;

    // todo setDirection here, for how many players are ready

    // port -> (session_id, index in clients vector)
    std::unordered_map<in_port_t, std::pair<uint64_t, size_t>> clientsMap;


    void generateNewGame();

    void generatePixel(uint8_t playerNumber, uint32_t x, uint32_t y);

    void generatePlayerEliminated(uint8_t playerNumber);

    void generateGameOver();

    void checkNewPosition(size_t index);

    void round();

    void sendNewEvent(size_t index);

public:
    explicit GameState(ArgumentsParserServer const &argumentParser);

    ~GameState();

    void startGame();

    void roundsForSecond();

    void playGame();

    void gameOver();

    void waitForClients();

    void setPlayerReady(in_port_t port);

    [[nodiscard]] Direction &addClient(int usingSocket, struct sockaddr_in client_address, uint64_t sessionId,
                                       std::string playerName);

    [[nodiscard]] EventHistory const &getEvents() const {
        return events_history;
    }

    [[nodiscard]] uint32_t getGameId() const {
        return game_id;
    }

    [[nodiscard]] uint32_t getNewestEventIndex() const {
        return events_history.size();
    }

    [[nodiscard]] bool hasGameEnded() const {
        return hasEnded;
    }

    [[nodiscard]] size_t connectedClients() const {
        return clients.size();
    }
};

#endif //DUZE_ZAD_GAME_STATE_H

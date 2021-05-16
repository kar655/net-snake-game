#include "game_state.h"

#include <iostream>
#include <cmath>


bool Position::move() {
    double const directionRadians = static_cast<double>(directionDegree) * M_PI / 180.0;
    x += std::sin(directionRadians);
    y += std::cos(directionRadians);

    Pixel newPixel(x, y);
    bool const hasChanged = !(lastPixel == newPixel);
    lastPixel = newPixel;

    return hasChanged;
}

void GameState::startGame() {
    game_id = randomNumberGenerator.generate();
    std::cout << "NEW GAME" << std::endl; // todo

    for (size_t i = 0; i < clients.size(); ++i) {
        players_positions.emplace_back(
                static_cast<double>(randomNumberGenerator.generate() % maxx) + 0.5,
                static_cast<double>(randomNumberGenerator.generate() % maxy) + 0.5,
                randomNumberGenerator.generate() % 360
        );

        Pixel const &pixelAssigned = players_positions.back().lastPixel;
        if (eaten[pixelAssigned.x][pixelAssigned.y]) {
            std::cout << "Pixel Already eaten. Sending PLAYER_ELIMINATED" << std::endl;
        }
        else {
            eaten[pixelAssigned.x][pixelAssigned.y] = true;
            std::cout << "Eating pixel. Sending PIXEL" << std::endl;
        }
    }
}

void GameState::round() {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (players_positions[i].direction == RIGHT) {
            players_positions[i].directionDegree += turningSpeed;
        }
        else if (players_positions[i].direction == LEFT) {
            players_positions[i].directionDegree -= turningSpeed;
        }

        players_positions[i].directionDegree %= 360;

        if (players_positions[i].move()) {
            std::cout << "Pixel Already eaten. Sending PLAYER_ELIMINATED" << std::endl;
        }
        else {
            std::cout << "Eating pixel. Sending PIXEL" << std::endl;
        }
    }
}

void GameState::gameOver() {

}

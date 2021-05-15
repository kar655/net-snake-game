#ifndef DUZE_ZAD_ARGUMENTS_PARSER_CLIENT_H
#define DUZE_ZAD_ARGUMENTS_PARSER_H

#include <string>
#include <cstdint>
#include <iostream>

class ArgumentsParserServer {
private:
    std::string options = "p:s:t:v:w:h:";
    uint_fast16_t port = 2021;
    uint_fast32_t seed = time(nullptr);
    uint_fast16_t turningSpeed = 6;
    uint_fast16_t roundsPerSecond = 50;
    uint_fast32_t width = 640;
    uint_fast32_t height = 480;

public:
    ArgumentsParserServer(int argc, char *argv[]);

    [[nodiscard]] uint_fast16_t getPort() const {
        return port;
    }

    [[nodiscard]] uint_fast32_t getSeed() const {
        return seed;
    }

    [[nodiscard]] uint_fast16_t getTurningSpeed() const {
        return turningSpeed;
    }

    [[nodiscard]] uint_fast16_t getRoundPerSecond() const {
        return roundsPerSecond;
    }

    [[nodiscard]] uint_fast32_t getWidth() const {
        return width;
    }

    [[nodiscard]] uint_fast32_t getHeight() const {
        return height;
    }
};

std::ostream &operator<<(std::ostream &os, ArgumentsParserServer const &argumentsParser);


#endif //DUZE_ZAD_ARGUMENTS_PARSER_CLIENT_H

#ifndef DUZE_ZAD_ARGUMENTS_PARSER_CLIENT_H
#define DUZE_ZAD_ARGUMENTS_PARSER_CLIENT_H

#include <string>
#include <cstdint>
#include <iostream>

class ArgumentsParserServer {
private:
    std::string options = "p:s:t:v:w:h:";
    uint16_t port = 2021;
    uint32_t seed = time(nullptr);
    uint16_t turningSpeed = 6;
    uint16_t roundsPerSecond = 50;
    uint32_t width = 640;
    uint32_t height = 480;

    void checkNegative(int parsed);

public:
    ArgumentsParserServer(int argc, char *argv[]);

    [[nodiscard]] uint16_t getPort() const {
        return port;
    }

    [[nodiscard]] uint32_t getSeed() const {
        return seed;
    }

    [[nodiscard]] uint16_t getTurningSpeed() const {
        return turningSpeed;
    }

    [[nodiscard]] uint16_t getRoundsPerSecond() const {
        return roundsPerSecond;
    }

    [[nodiscard]] uint32_t getWidth() const {
        return width;
    }

    [[nodiscard]] uint32_t getHeight() const {
        return height;
    }
};

std::ostream &operator<<(std::ostream &os, ArgumentsParserServer const &argumentsParser);


#endif //DUZE_ZAD_ARGUMENTS_PARSER_CLIENT_H

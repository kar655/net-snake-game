#include "steering.h"

#include <cstdio>
#include <iostream>
#include <termios.h>

#define STDIN_FILENO 0

namespace {
    uint_fast8_t constexpr LEFT = 3;
    int constexpr FIRST_ARROW_CHARACTER = 27;
    int constexpr SECOND_ARROW_CHARACTER = 91;
    int constexpr LEFT_ARROW = 68;
    int constexpr RIGHT_ARROW = 67;
}


void stopBuffering() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void parseMove() {
    int read = std::getchar();

    if (read == FIRST_ARROW_CHARACTER) {
        read = std::getchar();
        if (read == SECOND_ARROW_CHARACTER) {
            read = std::getchar();
            switch (read) {
                case LEFT_ARROW:
                    std::cout << "LEFT" << std::endl;
                    return;
                case RIGHT_ARROW:
                    std::cout << "RIGHT" << std::endl;
                    return;
                default:
                    std::cout << "Unknown arrow" << std::endl;
            }
        }
    }

    std::cout << "Got " << read
              << " as char " << static_cast<unsigned char>(read) << std::endl;
}

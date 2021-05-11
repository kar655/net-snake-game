#ifndef DUZE_ZAD_STEERING_H
#define DUZE_ZAD_STEERING_H

#include <cstdint>

enum Direction : uint_fast8_t {
    STRAIGHT = 0,
    RIGHT = 1,
    LEFT = 2,
};

//class MoveParser {
//
//};

void stopBuffering();

void parseMove();

#endif //DUZE_ZAD_STEERING_H

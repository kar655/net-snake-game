#include "RandomNumberGenerator.h"

uint32_t RandomNumberGenerator::generate() {
    if (running) {
        current_value = (current_value * multiply) % modulo;
    }
    else {
        running = true;
    }

    return current_value;
}

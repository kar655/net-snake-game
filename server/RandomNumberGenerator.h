#ifndef DUZE_ZAD_RANDOMNUMBERGENERATOR_H
#define DUZE_ZAD_RANDOMNUMBERGENERATOR_H

#include <cstdint>
#include <ctime>

class RandomNumberGenerator {
private:
    static uint_fast64_t constexpr modulo = 4'294'967'291;
    static uint_fast64_t constexpr multiply = 279'410'273;
    uint_fast64_t const seed;
    uint_fast64_t current_value;
    bool running = false;

public:
    explicit RandomNumberGenerator(
            uint_fast32_t new_seed = static_cast<uint_fast32_t>(std::time(nullptr))
    )
            : seed(static_cast<uint_fast64_t>(new_seed)), current_value(seed) {}

    [[nodiscard]] uint_fast32_t generate() {
        if (running) {
            current_value = (current_value * multiply) % modulo;
        }
        else {
            running = true;
        }

        return current_value;
    }
};

#endif //DUZE_ZAD_RANDOMNUMBERGENERATOR_H

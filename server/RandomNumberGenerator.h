#ifndef DUZE_ZAD_RANDOMNUMBERGENERATOR_H
#define DUZE_ZAD_RANDOMNUMBERGENERATOR_H

#include <cstdint>
#include <ctime>

class RandomNumberGenerator {
private:
    static uint64_t constexpr modulo = 4'294'967'291;
    static uint64_t constexpr multiply = 279'410'273;
    uint64_t const seed;
    uint64_t current_value;
    bool running = false;

public:
    explicit RandomNumberGenerator(uint32_t new_seed)
            : seed(static_cast<uint64_t>(new_seed)), current_value(seed) {}

    [[nodiscard]] uint32_t generate();
};

#endif //DUZE_ZAD_RANDOMNUMBERGENERATOR_H

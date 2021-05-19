#include "control_sum.h"

namespace ControlSum {
    [[nodiscard]] uint32_t crc32Check(void const *data, std::size_t size) {
        // TODO
        uint32_t crc = 0xFFFFFFFF;

        auto dataBytes = static_cast<uint8_t const *>(data);

        for (std::size_t i = 0; i < size; i++) {
            crc = crc ^ *(dataBytes + i);

            for (int j = 7; j >= 0; j--) {
                uint32_t mask = -(crc & 1);
                crc = (crc >> 1) ^ (0xEDB88320 & mask);
            }
        }

        return ~crc;
    }
} // namespace ControlSum

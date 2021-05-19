#ifndef DUZE_ZAD_CONTROL_SUM_H
#define DUZE_ZAD_CONTROL_SUM_H

#include <cstdint>

namespace ControlSum {
    [[nodiscard]] uint32_t crc32Check(void const *data, std::size_t size);

} // namespace ControlSum

#endif //DUZE_ZAD_CONTROL_SUM_H

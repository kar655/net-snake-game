#ifndef DUZE_ZAD_SERIALIZABLE_H
#define DUZE_ZAD_SERIALIZABLE_H

#include <string>

class Serializable {
public:
    [[nodiscard]] virtual std::string serialize() const = 0;
};

#endif //DUZE_ZAD_SERIALIZABLE_H

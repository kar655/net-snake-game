#ifndef DUZE_ZAD_CLIENT_MESSAGES_H
#define DUZE_ZAD_CLIENT_MESSAGES_H

#include <iostream>
#include <cstdint>
#include <string>
#include "serializable.h"

struct ClientMessage : public Serializable {
    uint_fast64_t session_id;
    uint_fast8_t turn_direction;
    uint_fast32_t next_expected_event_no;
//    std::string player_name;
    unsigned char player_name[20];

    [[nodiscard]] std::string serialize() const override;
};

std::ostream &operator<<(std::ostream &os, ClientMessage const &clientMessage);


struct ServerMessage {
    uint_fast32_t game_id;
    // TODO events
};


#endif //DUZE_ZAD_CLIENT_MESSAGES_H

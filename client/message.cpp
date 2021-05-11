#include "message.h"

std::ostream &operator<<(std::ostream &os, ClientMessage const &clientMessage) {
    return os << "session_id=" << clientMessage.session_id
              << "    turn_direction=" << clientMessage.turn_direction
              << "    next_expected_event_no=" << clientMessage.next_expected_event_no
              << "    player_name=" << clientMessage.player_name;
}

#include "messages.h"

std::ostream &operator<<(std::ostream &os, ClientMessage const &clientMessage) {
    return os << "session_id=" << clientMessage.session_id
              << "    turn_direction=" << static_cast<uint_fast32_t>(clientMessage.turn_direction)
              << "    next_expected_event_no=" << clientMessage.next_expected_event_no
              << "    player_name=" << clientMessage.player_name;
}

std::ostream &operator<<(std::ostream &os, Event const &event) {
    os << "len=" << event.len
       << "    event_no=" << event.event_no
       << "    event_type=" << static_cast<uint_fast32_t>(event.event_type);

    // event_data

    os << "    cec32=" << event.cec32;

    return os;
}

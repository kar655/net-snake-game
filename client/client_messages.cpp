#include <sstream>
#include <iomanip>
#include "client_messages.h"

std::ostream &operator<<(std::ostream &os, ClientMessage const &clientMessage) {
    return os << "session_id=" << clientMessage.session_id
              << "    turn_direction=" << static_cast<uint_fast32_t>(clientMessage.turn_direction)
              << "    next_expected_event_no=" << clientMessage.next_expected_event_no
              << "    player_name=" << clientMessage.player_name;
}

std::string ClientMessage::serialize() const {
    std::stringstream ss;
    ss
//            << std::setfill('\0')
            << session_id
            << turn_direction
            << next_expected_event_no
            << std::setw(20) << player_name;
    return ss.str();
}

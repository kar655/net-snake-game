#include "messages.h"

std::ostream &operator<<(std::ostream &os, ClientMessage const &clientMessage) {
    return os << "session_id=" << clientMessage.session_id
              << "    turn_direction=" << static_cast<uint_fast32_t>(clientMessage.turn_direction)
              << "    next_expected_event_no=" << clientMessage.next_expected_event_no
              << "    player_name=" << clientMessage.player_name;
}

std::ostream &operator<<(std::ostream &os, EventNewGame const &event) {
    return os << "EventNewGame";
}

std::ostream &operator<<(std::ostream &os, EventPixel const &event) {
    return os << "EventPixel";
}

std::ostream &operator<<(std::ostream &os, EventPlayerEliminated const &event) {
    return os << "EventPlayerEliminated";
}

std::ostream &operator<<(std::ostream &os, EventGameOver const &event) {
    return os << "EventGameOver";
}

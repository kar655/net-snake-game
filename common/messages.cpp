#include "messages.h"

std::ostream &operator<<(std::ostream &os, ClientMessage const &clientMessage) {
    return os << "session_id=" << clientMessage.session_id
              << "    turn_direction=" << static_cast<uint_fast32_t>(clientMessage.turn_direction)
              << "    next_expected_event_no=" << clientMessage.next_expected_event_no
              << "    player_name=" << clientMessage.player_name;
}

std::ostream &operator<<(std::ostream &os, EventNewGame const &event) {
    return os << "EventNewGame len=" << event.len << " number=" << event.event_no
              << " maxx=" << event.maxx << " maxy=" << event.maxy << " cec32=" << event.cec32;
}

std::ostream &operator<<(std::ostream &os, EventPixel const &event) {
    return os << "EventPixel len=" << event.len << " number=" << event.event_no
              << " x=" << event.x << " y=" << event.y << " cec32=" << event.cec32;
}

std::ostream &operator<<(std::ostream &os, EventPlayerEliminated const &event) {
    return os << "EventPlayerEliminated len=" << event.len << " number=" << event.event_no
              << " player_number=" << static_cast<int>(event.player_number)
              << " cec32=" << event.cec32;
}

std::ostream &operator<<(std::ostream &os, EventGameOver const &event) {
    return os << "EventGameOver len=" << event.len << " number=" << event.event_no
              << " cec32=" << event.cec32;
}

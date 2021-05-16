#include "messages.h"

std::ostream &operator<<(std::ostream &os, ClientMessage const &clientMessage) {
    return os << "session_id=" << clientMessage.session_id
              << "    turn_direction=" << static_cast<uint_fast32_t>(clientMessage.turn_direction)
              << "    next_expected_event_no=" << clientMessage.next_expected_event_no
              << "    player_name=" << clientMessage.player_name;
}

std::ostream &operator<<(std::ostream &os, Event const &event) {
//    os << "len=" << event.len
//       << "    event_no=" << event.event_no
//       << "    event_type=" << static_cast<uint_fast32_t>(event.event_type);
//
//    // event_data
//    if (event.event_type == NEW_GAME) {
//        os << "    NEW_GAME";
//    }
//    else if (event.event_type == PIXEL) {
//        os << "    PIXEL";
//    }
//    else if (event.event_type == PLAYER_ELIMINATED) {
//        os << "    PLAYER_ELIMINATED";
////        EventPlayerEliminated playerEliminated
////                = *reinterpret_cast<EventPlayerEliminated*>(event.event_data);
////        os << "    " << playerEliminated.player_number;
//        os << "    " << (uint8_t) event.event_data;
//    }
//    else if (event.event_type == GAME_OVER) {
//        os << "    GAME_OVER";
//    }
//
//    os << "    cec32=" << event.cec32;

    return os << "Event";
}

std::ostream &operator<<(std::ostream &os, EventPlayerEliminated const &event) {
    return os << "EventPlayerEliminated";
}
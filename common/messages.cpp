#include "messages.h"

ClientMessageWrapper::ClientMessageWrapper(std::string const &playerName)
        : size(sizeof(ClientMessage) + playerName.size()) {
    ClientMessage message;
    message.session_id = htobe64(message.session_id);

    clientMessageData = std::malloc(size);

    std::memcpy(clientMessageData, &message, sizeof(ClientMessage));
    std::memcpy(static_cast<char *>(clientMessageData) + sizeof(ClientMessage),
                playerName.c_str(), playerName.size());
}

ClientMessageWrapper::ClientMessageWrapper(void *clientMessageData, size_t size)
        : clientMessageData(clientMessageData), size(size) {}


std::ostream &operator<<(std::ostream &os, ClientMessage const &clientMessage) {
    return os << "session_id=" << clientMessage.session_id
              << "    turn_direction=" << static_cast<uint_fast32_t>(clientMessage.turn_direction)
              << "    next_expected_event_no=" << clientMessage.next_expected_event_no;
//              << "    player_name=" << clientMessage.player_name;
}

std::ostream &operator<<(std::ostream &os, ClientMessageWrapper const &clientMessage) {
    return os << "WRAPPER " << "session_id=" << clientMessage.getSessionId()
              << "    turn_direction=" << static_cast<int>(clientMessage.getTurnDirection())
              << "    next_expected_event_no=" << clientMessage.getEventNumber()
              << "    player_name=" << clientMessage.getPlayerName()
              << "    size=" << clientMessage.getSize();
}

std::ostream &operator<<(std::ostream &os, EventNewGame const &event) {
    return os << "EventNewGame len=" << event.len << " number=" << event.event_no
              << " maxx=" << event.maxx << " maxy=" << event.maxy;
//              << " crc32=" << event.crc32;
}

std::ostream &operator<<(std::ostream &os, EventPixel const &event) {
    return os << "EventPixel len=" << event.len << " number=" << event.event_no
              << " x=" << event.x << " y=" << event.y << " crc32=" << event.crc32;
}

std::ostream &operator<<(std::ostream &os, EventPlayerEliminated const &event) {
    return os << "EventPlayerEliminated len=" << event.len << " number=" << event.event_no
              << " player_number=" << static_cast<int>(event.player_number)
              << " crc32=" << event.crc32;
}

std::ostream &operator<<(std::ostream &os, EventGameOver const &event) {
    return os << "EventGameOver len=" << event.len << " number=" << event.event_no
              << " crc32=" << event.crc32;
}

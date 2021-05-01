#ifndef DUZE_ZAD_ARGUMENTS_PARSER_H
#define DUZE_ZAD_ARGUMENTS_PARSER_H

#include <string>
#include <cstdint>

class ArgumentsParser {
private:
    std::string options = "n:p:i:r:";
    std::string game_server;
    std::string player_name;
    uint_fast16_t server_port = 2021;
    std::string gui_server = "localhost";
    uint_fast16_t gui_port = 20210;
public:
    ArgumentsParser(int argc, char *argv[]);

    [[nodiscard]] std::string getGameServer() const {
        return game_server;
    }

    [[nodiscard]] std::string getPlayerName() const {
        return player_name;
    }

    [[nodiscard]] uint_fast16_t getServerPort() const {
        return server_port;
    }

    [[nodiscard]] std::string getGuiServer() const {
        return gui_server;
    }

    [[nodiscard]] uint_fast16_t getGuiPort() const {
        return gui_port;
    }
};

#endif //DUZE_ZAD_ARGUMENTS_PARSER_H

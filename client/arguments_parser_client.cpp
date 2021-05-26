#include <getopt.h>
#include <iostream>
#include "arguments_parser_client.h"

ArgumentsParserClient::ArgumentsParserClient(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Wrong number of arguments" << std::endl;
        exit(1);
    }

    game_server = argv[1];

    int opt, parsed;

    while ((opt = getopt(argc, argv, options.c_str())) != -1) {
        switch (opt) {
            case 'n':
                player_name = optarg;

                if (player_name.size() > 20) {
                    std::cerr << "Incorrect player name" << std::endl;
                    exit(1);
                }

                for (char c : player_name) {
                    if (c < 33 || c > 126) {
                        std::cerr << "Incorrect player name" << std::endl;
                        exit(1);
                    }
                }

                break;
            case 'p':
                parsed = std::stoi(optarg);
                if (parsed <= 0) {
                    std::cerr << "Incorrect server port" << std::endl;
                    exit(1);
                }
                break;
            case 'i':
                gui_server = optarg;
                break;
            case 'r':
                parsed = std::stoi(optarg);
                if (parsed <= 0) {
                    std::cerr << "Incorrect gui port" << std::endl;
                }
                gui_port = parsed;
                break;
            case '?':
            default:
                std::cerr << "Incorrect option" << std::endl;
                exit(1);
        }
    }
}


std::ostream &operator<<(std::ostream &os, ArgumentsParserClient const &argumentsParser) {
    return os << "game_server: " << argumentsParser.getGameServer()
              << "    player_name: " << argumentsParser.getPlayerName()
              << "    server_port: " << argumentsParser.getServerPort()
              << "    gui_server: " << argumentsParser.getGuiServer()
              << "    gui_port: " << argumentsParser.getGuiPort();
}

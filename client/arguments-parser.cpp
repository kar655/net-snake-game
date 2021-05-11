#include <getopt.h>
#include <iostream>
#include "arguments-parser.h"

ArgumentsParser::ArgumentsParser(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Wrong number of arguments" << std::endl;
        exit(1);
    }

    game_server = argv[1];

    int opt;

    while ((opt = getopt(argc, argv, options.c_str())) != -1) {
        switch (opt) {
            case 'n':
                player_name = optarg;
                break;
            case 'p':
                server_port = std::stoi(optarg);
                break;
            case 'i':
                gui_server = optarg;
                break;
            case 'r':
                gui_port = std::stoi(optarg);
//                std::cout << "Got for " << (char) opt << " = "
//                          << (optarg == nullptr ? "NULL" : optarg) << std::endl;
                break;
            case '?':
            default:
                std::cerr << "Unknown option: " << static_cast<char>(opt) << std::endl;
                exit(1);
        }
    }
}


std::ostream &operator<<(std::ostream &os, ArgumentsParser const &argumentsParser) {
    return os << "game_server: " << argumentsParser.getGameServer()
              << "    player_name: " << argumentsParser.getPlayerName()
              << "    server_port: " << argumentsParser.getServerPort()
              << "    gui_server: " << argumentsParser.getGuiServer()
              << "    gui_port: " << argumentsParser.getGuiPort();
}

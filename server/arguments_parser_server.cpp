#include <getopt.h>

#include "arguments_parser_server.h"

ArgumentsParserServer::ArgumentsParserServer(int argc, char **argv) {
    int opt, parsed;

    while ((opt = getopt(argc, argv, options.c_str())) != -1) {
        switch (opt) {
            case 'p':
                parsed = std::stoi(optarg);
                checkNegative(parsed);
                port = parsed;
                break;
            case 's':
                parsed = std::stoi(optarg);
                checkNegative(parsed);
                seed = parsed;
                break;
            case 't':
                parsed = std::stoi(optarg);
                checkNegative(parsed);
                turningSpeed = parsed;
                break;
            case 'v':
                parsed = std::stoi(optarg);
                checkNegative(parsed);
                roundsPerSecond = parsed;
                break;
            case 'w':
                parsed = std::stoi(optarg);
                checkNegative(parsed);
                width = parsed;
                break;
            case 'h':
                parsed = std::stoi(optarg);
                checkNegative(parsed);
                height = parsed;
                break;
            case '?':
            default:
                std::cerr << "Unknown option: " << static_cast<char>(opt) << std::endl;
                exit(1);
        }
    }
}

void ArgumentsParserServer::checkNegative(int parsed) {
    if (parsed <= 0) {
        std::cerr << "Incorrect argument" << std::endl;
        exit(1);
    }
}

std::ostream &operator<<(std::ostream &os, ArgumentsParserServer const &argumentsParser) {
    return os << "port: " << argumentsParser.getPort()
              << "    seed: " << argumentsParser.getSeed()
              << "    turningSpeed: " << argumentsParser.getTurningSpeed()
              << "    roundsPerSecond: " << argumentsParser.getRoundsPerSecond()
              << "    width: " << argumentsParser.getWidth()
              << "    height: " << argumentsParser.getHeight();
}

#include <getopt.h>
#include "arguments_parser_server.h"

ArgumentsParserServer::ArgumentsParserServer(int argc, char **argv) {
    int opt;

    while ((opt = getopt(argc, argv, options.c_str())) != -1) {
        switch (opt) {
            case 'p':
                port = std::stoi(optarg);
                break;
            case 's':
                seed = std::stoi(optarg);
                break;
            case 't':
                turningSpeed = std::stoi(optarg);
                if (turningSpeed == 0) {
                    exit(1);
                }
                break;
            case 'v':
                roundsPerSecond = std::stoi(optarg);
                if (roundsPerSecond == 0) {
                    exit(1);
                }
                break;
            case 'w':
                width = std::stoi(optarg);
                if (width == 0) {
                    exit(1);
                }
                break;
            case 'h':
                height = std::stoi(optarg);
                if (height == 0) {
                    exit(1);
                }
                break;
            case '?':
            default:
                std::cerr << "Unknown option: " << static_cast<char>(opt) << std::endl;
                exit(1);
        }
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

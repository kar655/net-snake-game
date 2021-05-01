#include <iostream>
#include "arguments-parser.h"

int main(int argc, char *argv[]) {
    std::cout << "Hello" << std::endl;

    const ArgumentsParser argumentParser(argc, argv);

    std::cout << "game_server: " << argumentParser.getGameServer()
              << "\tplayer_name: " << argumentParser.getPlayerName()
              << "\tserver_port: " << argumentParser.getServerPort()
              << "\tgui_server: " << argumentParser.getGuiServer()
              << "\tgui_port: " << argumentParser.getGuiPort() << std::endl;

    return 0;
}

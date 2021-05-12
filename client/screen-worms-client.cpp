#include <iostream>
#include "arguments-parser.h"
#include "steering.h"
#include "client_connection.h"

struct start_config {
    uint_fast32_t maxx;
    uint_fast32_t maxy;
    std::vector<std::string> playerNames;

    start_config()
            : maxx(50), maxy(30),
              playerNames({
                                  "FirstPlayer",
                                  "SecondPlayer",
                          }) {}
};

int main(int argc, char *argv[]) {
    start_config startConfig;
    std::cout << "Hello" << std::endl;

    uint_fast64_t session_id = time(nullptr);
    std::cout << "Current session_id = " << session_id << std::endl;

    const ArgumentsParser argumentsParser(argc, argv);

    std::cout << argumentsParser << std::endl;

//    stopBuffering();
//    while (true) {
//        parseMove();
//    }

    ClientToGUIConnection guiConnection(argumentsParser.getGuiServer(),
                                        argumentsParser.getGuiPort());

    guiConnection.initialMessage(startConfig.maxx, startConfig.maxy,
                                 startConfig.playerNames);

    guiConnection.startReading();

    return 0;
}

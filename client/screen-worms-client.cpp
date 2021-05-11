#include <iostream>
#include "arguments-parser.h"
#include "steering.h"

int main(int argc, char *argv[]) {
    std::cout << "Hello" << std::endl;

    uint_fast64_t session_id = time(nullptr);
    std::cout << "Current session_id = " << session_id << std::endl;

    const ArgumentsParser argumentsParser(argc, argv);

    std::cout << argumentsParser << std::endl;

    stopBuffering();
    while (true) {
        parseMove();
    }

    return 0;
}

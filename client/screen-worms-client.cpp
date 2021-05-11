#include <iostream>
#include "arguments-parser.h"
#include "Steering.h"

int main(int argc, char *argv[]) {
    std::cout << "Hello" << std::endl;

    const ArgumentsParser argumentsParser(argc, argv);

    std::cout << argumentsParser << std::endl;

    stopBuffering();
    while (true) {
        parseMove();
    }

    return 0;
}

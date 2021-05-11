cmake_minimum_required(VERSION 3.10)
project(duze_zad)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra") # TODO -O2

add_executable(screen-worms-client
        client/screen-worms-client.cpp
        client/arguments-parser.cpp
        client/arguments-parser.h
        client/message.cpp
        client/message.h
        client/Steering.cpp
        client/Steering.h)

add_executable(screen-worms-server
        server/screen-worms-server.cpp
        server/RandomNumberGenerator.cpp
        server/RandomNumberGenerator.h)

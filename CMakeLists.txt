cmake_minimum_required(VERSION 3.10)
project(duze_zad)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra") # TODO -O2

add_executable(screen-worms-client
        client/screen-worms-client.cpp
        client/arguments_parser_client.cpp client/arguments_parser_client.h
        client/client_messages.cpp client/client_messages.h
        client/steering.cpp client/steering.h
        client/serializable.h
        client/client_connection.cpp client/client_connection.h)

add_executable(screen-worms-server
        server/screen-worms-server.cpp
        server/RandomNumberGenerator.cpp server/RandomNumberGenerator.h
        server/server_messages.cpp server/server_messages.h
        server/events.cpp server/events.h
        server/configuration.h
        server/game_state.cpp server/game_state.h
        server/server_connection.cpp server/server_connection.h
        server/arguments_parser_server.cpp server/arguments_parser_server.h)

cmake_minimum_required(VERSION 3.10)
project(duze_zad)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra") # TODO -O2

add_executable(screen-worms-client
        client/screen-worms-client.cpp
        client/arguments_parser_client.cpp client/arguments_parser_client.h
        client/steering.cpp client/steering.h
        client/client_connection.cpp client/client_connection.h
        client/client_messenger.cpp client/client_messenger.h
        common/messages.cpp common/messages.h
        common/control_sum.cpp common/control_sum.h)

add_executable(screen-worms-server
        server/screen-worms-server.cpp
        server/RandomNumberGenerator.cpp server/RandomNumberGenerator.h
        server/configuration.h
        server/game_state.cpp server/game_state.h
        server/server_connection.cpp server/server_connection.h
        server/arguments_parser_server.cpp server/arguments_parser_server.h
        common/messages.cpp common/messages.h
        common/control_sum.cpp common/control_sum.h)

target_link_libraries(screen-worms-server pthread)
target_link_libraries(screen-worms-client pthread)


CC = g++
CFLAGS  = -Wall -Wextra -std=c++17 -O2 -pthread


all: screen-worms-server screen-worms-client


screen-worms-server: server/screen-worms-server.cpp \
			random_number_generator.o \
			server/configuration.h \
			game_state.o \
			server_connection.o \
			arguments_parser_server.o \
			messages.o \
			control_sum.o
	$(CC) $(CFLAGS) -o screen-worms-server server/screen-worms-server.cpp \
		random_number_generator.o game_state.o server_connection.o \
		arguments_parser_server.o messages.o control_sum.o


screen-worms-client: client/screen-worms-client.cpp \
			arguments_parser_client.o \
			client_connection.o \
			client_messenger.o \
			messages.o \
			control_sum.o
	$(CC) $(CFLAGS) -o screen-worms-client client/screen-worms-client.cpp \
		arguments_parser_client.o client_connection.o client_messenger.o \
		messages.o control_sum.o


arguments_parser_client.o: client/arguments_parser_client.cpp client/arguments_parser_client.h
	$(CC) $(CFLAGS) -c client/arguments_parser_client.cpp

client_connection.o: client/client_connection.cpp client/client_connection.h
	$(CC) $(CFLAGS) -c client/client_connection.cpp

client_messenger.o: client/client_messenger.cpp client/client_messenger.h
	$(CC) $(CFLAGS) -c client/client_messenger.cpp

random_number_generator.o: server/random_number_generator.cpp server/random_number_generator.h
	$(CC) $(CFLAGS) -c server/random_number_generator.cpp

game_state.o: server/game_state.cpp server/game_state.h
	$(CC) $(CFLAGS) -c server/game_state.cpp

server_connection.o: server/server_connection.cpp server/server_connection.h
	$(CC) $(CFLAGS) -c server/server_connection.cpp

arguments_parser_server.o: server/arguments_parser_server.cpp server/arguments_parser_server.h
	$(CC) $(CFLAGS) -c server/arguments_parser_server.cpp

messages.o: common/messages.cpp common/messages.h
	$(CC) $(CFLAGS) -c common/messages.cpp

control_sum.o: common/control_sum.cpp common/control_sum.h
	$(CC) $(CFLAGS) -c common/control_sum.cpp


clean:
	$(RM) screen-worms-server screen-worms-client *.o

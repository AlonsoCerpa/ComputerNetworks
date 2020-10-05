all: client.exe server.exe

client.exe: client.cpp process_connection.cpp socket_utils.cpp user_interface.cpp game.cpp
	g++ client.cpp process_connection.cpp socket_utils.cpp user_interface.cpp game.cpp -o client.exe -pthread -lncurses

server.exe: server.cpp process_clientes.cpp socket_utils.cpp game.cpp
	g++ server.cpp process_clientes.cpp socket_utils.cpp game.cpp -o server.exe -pthread

clean:
	rm -f client.exe server.exe
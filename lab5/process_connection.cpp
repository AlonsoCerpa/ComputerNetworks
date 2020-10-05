#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>

#include "process_connection.h"
#include "socket_utils.h"

const int size_bytes_id_client = 3;
const int size_bytes_size_param = 3;

void process_from_server(int socketFD) {
    while (true) {
        int size0 = 1;
        char buffer0[size0];
        read_from_socket(socketFD, buffer0, size0);

        if (buffer0[0] == 'U') {  //Actualizacion de jugada o chat
            int size1 = size_bytes_id_client;
            char buffer1[size1];
            read_from_socket(socketFD, buffer1, size1);

            int size2 = 1;
            char buffer2[size2];
            read_from_socket(socketFD, buffer2, size2);

            if (buffer2[0] == 'M') {  //Se recibio una jugada
                int size3 = 1;
                char buffer3[size3];
                read_from_socket(socketFD, buffer3, size3);

                char movement = buffer3[0];
                int id_client_move = std::stoi(std::string(buffer1, size1));
                mtx_game.lock();
                game.move(movement, id_client_move, true);
                mtx_game.unlock();
            }
            else if (buffer2[0] == 'C') {  //Se recibio un mensaje de otro cliente hacia mi
                int size3 = size_bytes_size_param;
                char buffer3[size3];
                read_from_socket(socketFD, buffer3, size3);

                int size4 = std::stoi(std::string(buffer3, size3));
                char buffer4[size4];
                read_from_socket(socketFD, buffer4, size4);

                std::string msg(buffer4, size4);
                int id_client_src_msg = std::stoi(std::string(buffer1, size1));
                mtx_id_client_nickname_pieza.lock();
                std::string complete_msg = "[" + id_client_nickname_pieza[id_client_src_msg].first + "] -> ";
                mtx_id_client_nickname_pieza.unlock();
                mtx_nickname.lock();
                complete_msg += "[" + nickname + "]: ";
                mtx_nickname.unlock();
                complete_msg += msg + "\n";
                mtx_msgs_from_chat.lock();
                append_string_to_vector(complete_msg, msgs_from_chat);
                mtx_msgs_from_chat.unlock();
            }
            else if (buffer2[0] == 'c') {  //Se recibio un mensaje de mi hacia otro cliente
                int size3 = size_bytes_size_param;
                char buffer3[size3];
                read_from_socket(socketFD, buffer3, size3);

                int size4 = std::stoi(std::string(buffer3, size3));
                char buffer4[size4];
                read_from_socket(socketFD, buffer4, size4);

                std::string msg(buffer4, size4);
                int id_client_dest_msg = std::stoi(std::string(buffer1, size1));
                mtx_nickname.lock();
                std::string complete_msg = "[" + nickname + "] -> ";
                mtx_nickname.unlock();
                mtx_id_client_nickname_pieza.lock();
                complete_msg += "[" + id_client_nickname_pieza[id_client_dest_msg].first + "]: ";
                mtx_id_client_nickname_pieza.unlock();
                complete_msg += msg + "\n";
                mtx_msgs_from_chat.lock();
                append_string_to_vector(complete_msg, msgs_from_chat);
                mtx_msgs_from_chat.unlock();
            }
            else if (buffer2[0] == 'I') {  //Se obtuvo el id_client, ficha y nickname de otro cliente
                int size3 = 1;
                char buffer3[size3];
                read_from_socket(socketFD, buffer3, size3);

                int size4 = size_bytes_size_param;
                char buffer4[size4];
                read_from_socket(socketFD, buffer4, size4);

                int size5 = std::stoi(std::string(buffer4, size4));
                char buffer5[size5];
                read_from_socket(socketFD, buffer5, size5);

                int id_client_other_new = std::stoi(std::string(buffer1, size1));
                char new_pieza_other = buffer3[0];
                std::string new_nickname_other(buffer5, size5);
                mtx_id_client_nickname_pieza.lock();
                id_client_nickname_pieza[id_client_other_new].first = new_nickname_other;
                id_client_nickname_pieza[id_client_other_new].second = new_pieza_other;
                mtx_id_client_nickname_pieza.unlock();
                mtx_nickname_id_client.lock();
                nickname_id_client[new_nickname_other] = id_client_other_new;
                mtx_nickname_id_client.unlock();
                std::string complete_msg = "Se conecto un nuevo usuario con el nickname [" + new_nickname_other + "] y pieza [" + new_pieza_other + "]\n";
                mtx_msgs_from_chat.lock();
                append_string_to_vector(complete_msg, msgs_from_chat);
                mtx_msgs_from_chat.unlock();

                //Insertar el snake del jugador nuevo al juego de mi cliente
                int x_pos_start = 10 + (4 * id_client_other_new);
                int y_pos_start = 25;
                std::list<PosXY> snake_pos;
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-1));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-2));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-3));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-4));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-5));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-6));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-7));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-8));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-9));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-10));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-11));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-12));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-13));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-14));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-15));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-16));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-17));
                mtx_game.lock();
                game.ids_clients_snakes[id_client_other_new] = Snake(snake_pos, new_pieza_other);
                mtx_game.unlock();
            }
            else if (buffer2[0] == 'i') {  //Se obtuvo tu id_client, ficha y nickname
                int size3 = 1;
                char buffer3[size3];
                read_from_socket(socketFD, buffer3, size3);

                int size4 = size_bytes_size_param;
                char buffer4[size4];
                read_from_socket(socketFD, buffer4, size4);

                int size5 = std::stoi(std::string(buffer4, size4));
                char buffer5[size5];
                read_from_socket(socketFD, buffer5, size5);

                int new_id_client = std::stoi(std::string(buffer1, size1));
                char new_character_snake = buffer3[0];
                std::string new_nickname = std::string(buffer5, size5);
                mtx_id_client.lock();
                id_client = new_id_client;
                mtx_id_client.unlock();
                mtx_character_snake.lock();
                character_snake = new_character_snake;
                mtx_character_snake.unlock();
                mtx_nickname.lock();
                nickname = new_nickname;
                mtx_nickname.unlock();
                mtx_id_client_nickname_pieza.lock();
                id_client_nickname_pieza[new_id_client].first = new_nickname;
                id_client_nickname_pieza[new_id_client].second = new_character_snake;
                mtx_id_client_nickname_pieza.unlock();
                mtx_nickname_id_client.lock();
                nickname_id_client[new_nickname] = new_id_client;
                mtx_nickname_id_client.unlock();
                std::string complete_msg = "Te registraste con el nickname [" + new_nickname + "] y la pieza [" + new_character_snake + "]\n";
                mtx_msgs_from_chat.lock();
                append_string_to_vector(complete_msg, msgs_from_chat);
                mtx_msgs_from_chat.unlock();

                //Insertar mi snake nueva al juego de mi cliente
                int x_pos_start = 10 + (4 * new_id_client);
                int y_pos_start = 25;
                std::list<PosXY> snake_pos;
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-1));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-2));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-3));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-4));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-5));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-6));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-7));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-8));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-9));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-10));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-11));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-12));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-13));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-14));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-15));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-16));
                snake_pos.push_back(PosXY(x_pos_start, y_pos_start-17));
                mtx_game.lock();
                game.ids_clients_snakes[new_id_client] = Snake(snake_pos, new_character_snake);
                mtx_game.unlock();
            }
            else if (buffer2[0] == 'R') {  //Se cambio el nickname de un cliente
                int size3 = size_bytes_size_param;
                char buffer3[size3];
                read_from_socket(socketFD, buffer3, size3);

                int size4 = std::stoi(std::string(buffer3, size3));
                char buffer4[size4];
                read_from_socket(socketFD, buffer4, size4);

                int id_client_change = std::stoi(std::string(buffer1, size1));
                std::string new_nickname(buffer4, size4);
                mtx_id_client_nickname_pieza.lock();
                std::string old_nickname = id_client_nickname_pieza[id_client_change].first;
                id_client_nickname_pieza[id_client_change].first = new_nickname;
                mtx_id_client_nickname_pieza.unlock();
                mtx_nickname_id_client.lock();
                nickname_id_client[new_nickname] = id_client_change;
                mtx_nickname_id_client.unlock();
                mtx_id_client.lock();
                int aux_id_client = id_client;
                mtx_id_client.unlock();
                std::string complete_msg;
                if (id_client_change == aux_id_client) {
                    mtx_nickname.lock();
                    nickname = new_nickname;
                    mtx_nickname.unlock();
                    complete_msg = "Se cambio tu nickname de [" + old_nickname + "] a [" + new_nickname + "]\n";
                }
                else 
                    complete_msg = "Usuario [" + old_nickname + "] cambio su nickname a [" + new_nickname + "]\n";
                mtx_msgs_from_chat.lock();
                append_string_to_vector(complete_msg, msgs_from_chat);
                mtx_msgs_from_chat.unlock();
            }
            else if (buffer2[0] == 'L') {  //Alguien perdio
                int id_client_loser = std::stoi(std::string(buffer1, size1));
                mtx_id_client_nickname_pieza.lock();
                std::string nickname_loser = id_client_nickname_pieza[id_client_loser].first;
                mtx_id_client_nickname_pieza.unlock();
                mtx_id_client.lock();
                int my_id_client = id_client;
                mtx_id_client.unlock();
                std::string complete_msg;
                if (my_id_client == id_client_loser) {
                    complete_msg = "Tu jugador [" + nickname_loser + "] perdiste\n";
                    mtx_game.lock();
                    game.end_game = true;
                    mtx_game.unlock();
                }
                else
                    complete_msg = "El jugador [" + nickname_loser + "] perdio\n";
                mtx_msgs_from_chat.lock();
                append_string_to_vector(complete_msg, msgs_from_chat);
                mtx_msgs_from_chat.unlock();
            }
            else if (buffer2[0] == 'W') {  //Alguien gano
                int id_client_winner = std::stoi(std::string(buffer1, size1));
                mtx_id_client_nickname_pieza.lock();
                std::string nickname_winner = id_client_nickname_pieza[id_client_winner].first;
                mtx_id_client_nickname_pieza.unlock();
                mtx_id_client.lock();
                int my_id_client = id_client;
                mtx_id_client.unlock();
                std::string complete_msg;
                if (my_id_client == id_client_winner) {
                    complete_msg = "Tu jugador [" + nickname_winner + "] ganaste\n";
                    mtx_game.lock();
                    game.end_game = true;
                    mtx_game.unlock();
                }
                else
                    complete_msg = "El jugador [" + nickname_winner + "] gano\n";
                mtx_msgs_from_chat.lock();
                append_string_to_vector(complete_msg, msgs_from_chat);
                mtx_msgs_from_chat.unlock();
            }
            else if (buffer2[0] == 'F') {  //Alguien cambio su ficha
                
            }
        }
        else if (buffer0[0] == 'D') {  //Se desconecto otro cliente
            
        }
    }
}

void process_to_server(int socketFD) {
    std::vector<char> input;
    while (true) {
        mtx_inputs_to_server.lock();
        input = std::vector<char>();
        if (inputs_to_server.size() > 0) {
            input = inputs_to_server[0];
            inputs_to_server.erase(inputs_to_server.begin());
        }
        mtx_inputs_to_server.unlock();
        if (input.size() > 0) {
            if (input[0] == 'M') {
                char movement = input[1];
                mtx_id_client.lock();
                int my_id_client = id_client;
                mtx_id_client.unlock();
                bool valid_mov = game.valid_movement(movement, my_id_client);
                if (valid_mov == true) {
                    int size0 = 2;
                    char buffer0[size0] = {'M', movement};

                    write_to_socket(socketFD, buffer0, size0);
                }
            }
            else if (input[0] == 'C') {
                auto it0 = std::find(input.begin() + 2, input.end(), ' ');
                std::string nickname_dest(input.begin() + 2, it0);
                std::string msg_to_dest(it0 + 1, input.end());

                //Escribir al servidor el mensaje que quiero enviar a un usuario
                std::vector<std::pair<char *, int> > buffers_sizes0;

                int size0 = 1;
                char buffer0[size0] = {'C'};
                buffers_sizes0.push_back(std::pair<char *, int>(buffer0, size0));

                int size1 = size_bytes_id_client;
                char buffer1[size1];
                mtx_nickname_id_client.lock();
                std::string id_client_dest_str = std::to_string(nickname_id_client[nickname_dest]);
                mtx_nickname_id_client.unlock();
                copy_int_str_to_char_array(id_client_dest_str, buffer1, size1);
                buffers_sizes0.push_back(std::pair<char *, int>(buffer1, size1));

                int size2 = size_bytes_size_param;
                char buffer2[size2];
                std::string size_msg_dest_str = std::to_string(msg_to_dest.size());
                copy_int_str_to_char_array(size_msg_dest_str, buffer2, size2);
                buffers_sizes0.push_back(std::pair<char *, int>(buffer2, size2));

                buffers_sizes0.push_back(std::pair<char *, int>((char *) msg_to_dest.c_str(), msg_to_dest.size()));

                std::vector<char> buffer_write0;
                fill_buffer_w_buffers(buffer_write0, buffers_sizes0);

                write_to_socket(socketFD, &buffer_write0[0], buffer_write0.size());
            }
            else if (input[0] == 'R') {
                std::string new_nickname(input.begin() + 2, input.end());

                //Escribir al servidor mi nuevo nickname
                std::vector<std::pair<char *, int> > buffers_sizes0;

                int size0 = 1;
                char buffer0[size0] = {'R'};
                buffers_sizes0.push_back(std::pair<char *, int>(buffer0, size0));

                int size1 = size_bytes_size_param;
                char buffer1[size1];
                std::string size_new_nickname_str = std::to_string(new_nickname.size());
                copy_int_str_to_char_array(size_new_nickname_str, buffer1, size1);
                buffers_sizes0.push_back(std::pair<char *, int>(buffer1, size1));

                buffers_sizes0.push_back(std::pair<char *, int>((char *) new_nickname.c_str(), new_nickname.size()));

                std::vector<char> buffer_write0;
                fill_buffer_w_buffers(buffer_write0, buffers_sizes0);

                write_to_socket(socketFD, &buffer_write0[0], buffer_write0.size());
            }
            else if (input[0] == 'F') {

            }
            else {

            }
        }
    }
}

void process_connection(int num_jugadores) {
    int socketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (-1 == socketFD) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    sockaddr_in stSockAddr_juego;
    memset(&stSockAddr_juego, 0, sizeof(sockaddr_in));

    stSockAddr_juego.sin_family = AF_INET;
    stSockAddr_juego.sin_port = htons(50003);
    int Res = inet_pton(AF_INET, "51.15.220.108", &stSockAddr_juego.sin_addr);
    //127.0.0.1

    if (0 > Res) {
        perror("error: first parameter is not a valid address family");
        close(socketFD);
        exit(EXIT_FAILURE);
    }
    else if (0 == Res) {
        perror("char string (second parameter) does not contain valid ipaddress");
        close(socketFD);
        exit(EXIT_FAILURE);
    }

    if (-1 == connect(socketFD, (const struct sockaddr *)&stSockAddr_juego, sizeof(sockaddr_in))) {
        perror("connect failed");
        close(socketFD);
        exit(EXIT_FAILURE);
    }

    std::thread(process_from_server, socketFD).detach();
    std::thread(process_to_server, socketFD).detach();

    while(true) {

    }
}
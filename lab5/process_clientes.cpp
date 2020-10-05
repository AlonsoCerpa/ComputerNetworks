#include <iostream>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <mutex>
#include <unordered_map>
#include <string>
#include <algorithm>

#include "process_clientes.h"
#include "socket_utils.h"
#include "game.h"

std::unordered_map<int, int> id_client_socketFD;
std::unordered_map<int, std::string> id_client_nickname;
std::list<int> id_players_remaining;
int width_table = 78;
int height_table = 33;
Game game(width_table, height_table);

std::mutex mtx_print;
std::mutex mtx_id_client_socketFD;
std::mutex mtx_id_client_nickname;
std::mutex mtx_id_players_remaining;
std::mutex mtx_game;

const int size_bytes_id_client = 3;
const int size_bytes_size_param = 3;

std::vector<char> fichas = {'X', 'O', 'T', 'A', 'R', 'P', 'I', 'M', 'E', 'Z'};

void process_cliente(int ConnectFD, std::string nickname, int id_client)
{
    //Enviar a todos los clientes el id_cliente, la ficha y el nickname de mi cliente
    mtx_id_client_socketFD.lock();
    std::unordered_map<int, int> id_client_socketFD_copy = id_client_socketFD;
    mtx_id_client_socketFD.unlock();

    std::vector<std::pair<char *, int> > buffers_sizes0;

    int size0 = 1;
    char buffer0[size0] = {'U'};
    buffers_sizes0.push_back(std::pair<char *, int>(buffer0, size0));

    int size1 = size_bytes_id_client;
    char buffer1[size1];
    std::string id_client_new_str = std::to_string(id_client);
    copy_int_str_to_char_array(id_client_new_str, buffer1, size1);
    buffers_sizes0.push_back(std::pair<char *, int>(buffer1, size1));

    int size2 = 1;
    char buffer2[size2] = {'I'};
    buffers_sizes0.push_back(std::pair<char *, int>(buffer2, size2));

    int size3 = 1;
    char buffer3[size3] = {fichas[id_client]};
    buffers_sizes0.push_back(std::pair<char *, int>(buffer3, size3));

    int size4 = size_bytes_size_param;
    char buffer4[size4];
    std::string size_nickname_new_str = std::to_string(nickname.size());
    copy_int_str_to_char_array(size_nickname_new_str, buffer4, size4);
    buffers_sizes0.push_back(std::pair<char *, int>(buffer4, size4));

    buffers_sizes0.push_back(std::pair<char *, int>((char *) nickname.c_str(), nickname.size()));

    std::vector<char> buffer_write0;
    fill_buffer_w_buffers(buffer_write0, buffers_sizes0);

    for (auto it = id_client_socketFD_copy.begin(); it != id_client_socketFD_copy.end(); ++it) {
        if (it->first != id_client) {            
            write_to_socket(it->second, &buffer_write0[0], buffer_write0.size());
        }
        else {
            buffer_write0[1 + size_bytes_id_client] = 'i';
            write_to_socket(it->second, &buffer_write0[0], buffer_write0.size());
            buffer_write0[1 + size_bytes_id_client] = 'I';
        }
    }

    //Insertar el snake del jugador actual al juego del servidor
    int x_pos_start = 10 + (4 * id_client);
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
    game.ids_clients_snakes[id_client] = Snake(snake_pos, fichas[id_client]);
    mtx_game.unlock();

    //Enviar a mi cliente los id_clientes, las fichas y los nicknames de los demas clientes
    for (auto it = id_client_socketFD_copy.begin(); it != id_client_socketFD_copy.end(); ++it) {
        if (it->first != id_client) {
            std::vector<std::pair<char *, int> > buffers_sizes0;

            int size0 = 1;
            char buffer0[size0] = {'U'};
            buffers_sizes0.push_back(std::pair<char *, int>(buffer0, size0));

            int size1 = size_bytes_id_client;
            char buffer1[size1];
            std::string id_client_other_str = std::to_string(it->first);
            copy_int_str_to_char_array(id_client_other_str, buffer1, size1);
            buffers_sizes0.push_back(std::pair<char *, int>(buffer1, size1));

            int size2 = 1;
            char buffer2[size2] = {'I'};
            buffers_sizes0.push_back(std::pair<char *, int>(buffer2, size2));

            int size3 = 1;
            char buffer3[size3] = {fichas[it->first]};
            buffers_sizes0.push_back(std::pair<char *, int>(buffer3, size3));

            int size4 = size_bytes_size_param;
            char buffer4[size4];
            mtx_id_client_nickname.lock();
            std::string nickname_other = id_client_nickname[it->first];
            mtx_id_client_nickname.unlock();
            std::string size_nickname_other_str = std::to_string(nickname_other.size());
            copy_int_str_to_char_array(size_nickname_other_str, buffer4, size4);
            buffers_sizes0.push_back(std::pair<char *, int>(buffer4, size4));

            buffers_sizes0.push_back(std::pair<char *, int>((char *) nickname_other.c_str(), nickname_other.size()));

            std::vector<char> buffer_write0;
            fill_buffer_w_buffers(buffer_write0, buffers_sizes0);

            write_to_socket(ConnectFD, &buffer_write0[0], buffer_write0.size());
        }
    }

    while (true) {
        int size0 = 1;
        char buffer0[size0];
        read_from_socket(ConnectFD, buffer0, size0);

        if (buffer0[0] == 'M') {  //Jugada del cliente
            int size1 = 1;
            char buffer1[size1];
            read_from_socket(ConnectFD, buffer1, size1);

            char movement = buffer1[0];
            mtx_game.lock();
            bool moved = game.move(movement, id_client, false);
            mtx_game.unlock();
            if (moved == true) {    //Movimiento correcto
                std::vector<std::pair<char *, int> > buffers_sizes0;

                int size0 = 1;
                char buffer0[size0] = {'U'};
                buffers_sizes0.push_back(std::pair<char *, int>(buffer0, size0));

                int size1 = size_bytes_id_client;
                char buffer1[size1];
                std::string id_client_str = std::to_string(id_client);
                copy_int_str_to_char_array(id_client_str, buffer1, size1);
                buffers_sizes0.push_back(std::pair<char *, int>(buffer1, size1));

                int size2 = 2;
                char buffer2[size2] = {'M', movement};
                buffers_sizes0.push_back(std::pair<char *, int>(buffer2, size2));

                std::vector<char> buffer_write0;
                fill_buffer_w_buffers(buffer_write0, buffers_sizes0);

                mtx_id_client_socketFD.lock();
                std::unordered_map<int, int> id_client_socketFD_copy = id_client_socketFD;
                mtx_id_client_socketFD.unlock();

                for (auto it = id_client_socketFD_copy.begin(); it != id_client_socketFD_copy.end(); ++it) {         
                    write_to_socket(it->second, &buffer_write0[0], buffer_write0.size());
                }

                mtx_print.lock();
                std::cout << "Usuario [" << nickname << "] se movio '" << movement << "'\n"; 
                mtx_print.unlock();
            }
            else {     //Perdio al haber colision con otro snake o su mismo snake
                mtx_id_players_remaining.lock();
                auto it = std::find(id_players_remaining.begin(), id_players_remaining.end(), id_client);
                id_players_remaining.erase(it);
                mtx_id_players_remaining.unlock();

                std::vector<std::pair<char *, int> > buffers_sizes0;

                int size0 = 1;
                char buffer0[size0] = {'U'};
                buffers_sizes0.push_back(std::pair<char *, int>(buffer0, size0));

                int size1 = size_bytes_id_client;
                char buffer1[size1];
                std::string id_client_str = std::to_string(id_client);
                copy_int_str_to_char_array(id_client_str, buffer1, size1);
                buffers_sizes0.push_back(std::pair<char *, int>(buffer1, size1));

                int size2 = 1;
                char buffer2[size2] = {'L'};
                buffers_sizes0.push_back(std::pair<char *, int>(buffer2, size2));

                std::vector<char> buffer_write0;
                fill_buffer_w_buffers(buffer_write0, buffers_sizes0);

                mtx_id_client_socketFD.lock();
                std::unordered_map<int, int> id_client_socketFD_copy = id_client_socketFD;
                mtx_id_client_socketFD.unlock();

                for (auto it = id_client_socketFD_copy.begin(); it != id_client_socketFD_copy.end(); ++it) {         
                    write_to_socket(it->second, &buffer_write0[0], buffer_write0.size());
                }

                mtx_print.lock();
                std::cout << "Usuario [" << nickname << "] perdio\n"; 
                mtx_print.unlock();

                //Verificar si hay un ganador (ultimo jugador restante)
                mtx_id_players_remaining.lock();
                int size_id_player_rem = id_players_remaining.size();
                mtx_id_players_remaining.unlock();
                if (size_id_player_rem == 1) {
                    std::vector<std::pair<char *, int> > buffers_sizes0;

                    int size0 = 1;
                    char buffer0[size0] = {'U'};
                    buffers_sizes0.push_back(std::pair<char *, int>(buffer0, size0));

                    int size1 = size_bytes_id_client;
                    char buffer1[size1];
                    mtx_id_players_remaining.lock();
                    int id_client_winner = id_players_remaining.front();
                    mtx_id_players_remaining.unlock();
                    std::string id_client_winner_str = std::to_string(id_client_winner);
                    copy_int_str_to_char_array(id_client_winner_str, buffer1, size1);
                    buffers_sizes0.push_back(std::pair<char *, int>(buffer1, size1));

                    int size2 = 1;
                    char buffer2[size2] = {'W'};
                    buffers_sizes0.push_back(std::pair<char *, int>(buffer2, size2));

                    std::vector<char> buffer_write0;
                    fill_buffer_w_buffers(buffer_write0, buffers_sizes0);

                    mtx_id_client_socketFD.lock();
                    std::unordered_map<int, int> id_client_socketFD_copy = id_client_socketFD;
                    mtx_id_client_socketFD.unlock();

                    for (auto it = id_client_socketFD_copy.begin(); it != id_client_socketFD_copy.end(); ++it) {         
                        write_to_socket(it->second, &buffer_write0[0], buffer_write0.size());
                    }

                    mtx_id_players_remaining.lock();
                    id_players_remaining.erase(id_players_remaining.begin());
                    mtx_id_players_remaining.unlock();

                    mtx_id_client_nickname.lock();
                    std::string nickname_winner = id_client_nickname[id_client_winner];
                    mtx_id_client_nickname.unlock();

                    mtx_print.lock();
                    std::cout << "Usuario [" << nickname_winner << "] gano\n"; 
                    mtx_print.unlock();
                }
            }
        }
        else if (buffer0[0] == 'C') {   //Mensaje de chat de cliente
            //Leer el mensaje de mi cliente
            int size1 = size_bytes_id_client;
            char buffer1[size1]; 
            read_from_socket(ConnectFD, buffer1, size1);

            int size2 = size_bytes_size_param;
            char buffer2[size2];
            read_from_socket(ConnectFD, buffer2, size2);       

            int size3 = std::stoi(std::string(buffer2, size2));
            char buffer3[size3];
            read_from_socket(ConnectFD, buffer3, size3);

            std::string id_client_dest_str(buffer1, size1);
            int id_client_dest_int = std::stoi(id_client_dest_str);
            std::string msg(buffer3, size3);

            mtx_id_client_nickname.lock();
            std::string nickname_dest = id_client_nickname[id_client_dest_int];
            mtx_id_client_nickname.unlock();
            mtx_print.lock();
            std::cout << "Mensaje de usuario [" << nickname << "] a usuario [" << nickname_dest <<"]: " << msg << "\n";
            mtx_print.unlock();

            mtx_id_client_socketFD.lock();
            int socketFD_dest = id_client_socketFD[id_client_dest_int];
            mtx_id_client_socketFD.unlock();


            //Enviar mensaje al cliente destinatario
            std::vector<std::pair<char *, int> > buffers_sizes0;

            int size4 = 1;
            char buffer4[size4] = {'U'};
            buffers_sizes0.push_back(std::pair<char *, int>(buffer4, size4));

            std::string id_client_src_str = std::to_string(id_client);
            int size5 = size_bytes_id_client;
            char buffer5[size5];
            copy_int_str_to_char_array(id_client_src_str, buffer5, size5);
            buffers_sizes0.push_back(std::pair<char *, int>(buffer5, size5));
            
            int size6 = 1;
            char buffer6[size6] = {'C'};
            buffers_sizes0.push_back(std::pair<char *, int>(buffer6, size6));
 
            buffers_sizes0.push_back(std::pair<char *, int>(buffer2, size2));

            buffers_sizes0.push_back(std::pair<char *, int>(buffer3, size3));

            std::vector<char> buffer_write0;
            fill_buffer_w_buffers(buffer_write0, buffers_sizes0);

            write_to_socket(socketFD_dest, &buffer_write0[0], buffer_write0.size());


            //Enviarme mensaje a mi mismo
            std::vector<std::pair<char *, int> > buffers_sizes1;

            buffers_sizes1.push_back(std::pair<char *, int>(buffer4, size4));

            buffers_sizes1.push_back(std::pair<char *, int>(buffer1, size1));
            
            int size7 = 1;
            char buffer7[size7] = {'c'};
            buffers_sizes1.push_back(std::pair<char *, int>(buffer7, size7));
 
            buffers_sizes1.push_back(std::pair<char *, int>(buffer2, size2));

            buffers_sizes1.push_back(std::pair<char *, int>(buffer3, size3));

            std::vector<char> buffer_write1;
            fill_buffer_w_buffers(buffer_write1, buffers_sizes1);

            write_to_socket(ConnectFD, &buffer_write1[0], buffer_write1.size());
        }
        else if (buffer0[0] == 'R') {  //Cambio de nickname de cliente
            int size1 = size_bytes_size_param;
            char buffer1[size1];
            read_from_socket(ConnectFD, buffer1, size1);    

            int size2 = std::stoi(std::string(buffer1, size1));
            char buffer2[size2];
            read_from_socket(ConnectFD, buffer2, size2);

            std::string new_nickname(buffer2, size2);
            std::string old_nickname = nickname;
            nickname = new_nickname;
            mtx_id_client_nickname.lock();
            id_client_nickname[id_client] = new_nickname;
            mtx_id_client_nickname.unlock();

            mtx_print.lock();
            std::cout << "Usuario [" << old_nickname << "] cambio su nickname a [" << new_nickname << "]\n";
            mtx_print.unlock();


            //Enviar a todos los clientes el id_cliente y el nickname con el nuevo nickname
            std::vector<std::pair<char *, int> > buffers_sizes0;

            int size0 = 1;
            char buffer0[size0] = {'U'};
            buffers_sizes0.push_back(std::pair<char *, int>(buffer0, size0));

            int size3 = size_bytes_id_client;
            char buffer3[size3];
            std::string id_client_new_str = std::to_string(id_client);
            copy_int_str_to_char_array(id_client_new_str, buffer3, size3);
            buffers_sizes0.push_back(std::pair<char *, int>(buffer3, size3));

            int size4 = 1;
            char buffer4[size4] = {'R'};
            buffers_sizes0.push_back(std::pair<char *, int>(buffer4, size4));

            int size5 = size_bytes_size_param;
            char buffer5[size5];
            std::string size_nickname_new_str = std::to_string(new_nickname.size());
            copy_int_str_to_char_array(size_nickname_new_str, buffer5, size5);
            buffers_sizes0.push_back(std::pair<char *, int>(buffer5, size5));

            buffers_sizes0.push_back(std::pair<char *, int>((char *) new_nickname.c_str(), new_nickname.size()));

            std::vector<char> buffer_write0;
            fill_buffer_w_buffers(buffer_write0, buffers_sizes0);

            mtx_id_client_socketFD.lock();
            std::unordered_map<int, int> id_client_socketFD_copy = id_client_socketFD;
            mtx_id_client_socketFD.unlock();
    
            for (auto it = id_client_socketFD_copy.begin(); it != id_client_socketFD_copy.end(); ++it) {         
                write_to_socket(it->second, &buffer_write0[0], buffer_write0.size());
            }
        }
        else if (buffer0[0] == 'F') {  //Cambio de ficha de cliente
            
        }
    }

    shutdown(ConnectFD, SHUT_RDWR);
    close(ConnectFD);
}

void process_clientes(int num_jugadores) {
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;

    if(-1 == SocketFD) {
        perror("can not create socket");
        exit(EXIT_FAILURE);
    }

    sockaddr_in stSockAddr;
    memset(&stSockAddr, 0, sizeof(sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(50003);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    if(-1 == bind(SocketFD, (const sockaddr *)&stSockAddr, sizeof(sockaddr_in))) {
        perror("error bind failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if(-1 == listen(SocketFD, 10)) {
        perror("error listen failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    for(int id_client = 0; id_client < num_jugadores; ++id_client) {
        int ConnectFD = accept(SocketFD, NULL, NULL);

        if (0 > ConnectFD) {
            perror("error accept failed");
            close(SocketFD);
            exit(EXIT_FAILURE);
        }
        std::string nickname = "jugador" + std::to_string(id_client);

        mtx_id_client_socketFD.lock();
        id_client_socketFD[id_client] = ConnectFD;
        mtx_id_client_socketFD.unlock();

        mtx_id_client_nickname.lock();
        id_client_nickname[id_client] = nickname;
        mtx_id_client_nickname.unlock();

        mtx_id_players_remaining.lock();
        id_players_remaining.push_back(id_client);
        mtx_id_players_remaining.unlock();

        std::thread(process_cliente, ConnectFD, nickname, id_client).detach();
    }

    while (true) {

    }

    close(SocketFD);
}
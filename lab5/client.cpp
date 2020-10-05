#include <iostream>
#include <ncurses.h>
#include <thread>
#include <string>
#include <vector>
#include <queue>
#include <mutex>

#include "user_interface.h"
#include "process_connection.h"
#include "game.h"

std::vector<char> msgs_from_chat;
std::queue<std::vector<char> > queue_game;
std::string nickname;
char character_snake;
int id_client;
std::vector<std::vector<char> > inputs_to_server;
std::map<int, std::pair<std::string, char> > id_client_nickname_pieza;
std::unordered_map<std::string, int> nickname_id_client;
Game game;

std::mutex mtx_msgs_from_chat;
std::mutex mtx_queue_game;
std::mutex mtx_nickname;
std::mutex mtx_character_snake;
std::mutex mtx_id_client;
std::mutex mtx_inputs_to_server;
std::mutex mtx_id_client_nickname_pieza;
std::mutex mtx_nickname_id_client;
std::mutex mtx_game;

int main(int argc, char *argv[]) {
    int num_jugadores = std::stoi(std::string(argv[1]));

    nickname = "jugadorX";
    character_snake = '-';
    id_client = -1;

    int width_table = 78;
    int height_table = 33;
    game = Game(width_table, height_table);

    std::thread(process_connection, num_jugadores).detach();

    initscr();

    WINDOW *window_juego;
    WINDOW *window_chat;
    WINDOW *window_input;
    WINDOW *window_users;
    WINDOW *window_piezas;
    create_windows_for_game(&window_juego, &window_chat, &window_input, &window_users, &window_piezas);

    curs_set(0);
    noecho();
    nodelay(window_input, true);
    keypad(window_input, true);

    wprintw(window_chat, "Chat:");
    wprintw(window_input, "Input:");
    wprintw(window_users, "Nicknames:");
    wprintw(window_piezas, "Piezas:");

    wmove(window_input, 1, 0);
    wmove(window_chat, 1, 0);
    wmove(window_users, 1, 0);
    wmove(window_piezas, 1, 0);

    refresh();
    wrefresh(window_juego);
    wrefresh(window_chat);
    wrefresh(window_input);
    wrefresh(window_users);
    wrefresh(window_piezas);
    
    int character;
    std::vector<char> current_buffer;
    while (true) {
        //Leer input y procesar
        character = wgetch(window_input);
        if (character != -1) {
            mtx_game.lock();
            int current_num_players = game.ids_clients_snakes.size();
            bool end_game = game.end_game;
            mtx_game.unlock();
            if (character == KEY_BACKSPACE) {  //Se presiono Delete
                if (current_buffer.size() >= 1) {
                    current_buffer.pop_back();
                    int cursor_input_y, cursor_input_x;
                    getyx(window_input, cursor_input_y, cursor_input_x);
                    if (cursor_input_x == 0 and cursor_input_y >= 2) {
                        int w_input_width, w_input_height;
                        getmaxyx(window_input, w_input_height, w_input_width);
                        cursor_input_x = w_input_width - 1;
                        --cursor_input_y;
                    }
                    else {
                        --cursor_input_x;
                    }                    
                    wmove(window_input, cursor_input_y, cursor_input_x);
                    wdelch(window_input);
                }
            }
            else if (character == 10) {  //Se presiono Enter
                mtx_inputs_to_server.lock();
                inputs_to_server.push_back(current_buffer);
                mtx_inputs_to_server.unlock();
                current_buffer = std::vector<char>();

                int w_input_width, w_input_height;
                getmaxyx(window_input, w_input_height, w_input_width);
                int cursor_input_y, cursor_input_x;
                getyx(window_input, cursor_input_y, cursor_input_x);
                bool deletion = true;
                while (deletion) {
                    if (cursor_input_x > 0) {
                        --cursor_input_x;
                        wmove(window_input, cursor_input_y, cursor_input_x);
                        wdelch(window_input);
                    }
                    else {
                        if (cursor_input_y >= 2) {
                            --cursor_input_y;
                            cursor_input_x = w_input_width - 1;
                            wmove(window_input, cursor_input_y, cursor_input_x);
                            wdelch(window_input);
                        }
                        else
                            deletion = false;                        
                    }                    
                }
            }
            else if (character == KEY_UP) {  //Se presiono la flecha hacia arriba
                if (current_num_players == num_jugadores && end_game == false) {
                    std::vector<char> mov = {'M', 'U'};
                    mtx_inputs_to_server.lock();
                    inputs_to_server.push_back(mov);
                    mtx_inputs_to_server.unlock();
                }
            }
            else if (character == KEY_DOWN) {  //Se presiono la flecha hacia abajo
                if (current_num_players == num_jugadores && end_game == false) {
                    std::vector<char> mov = {'M', 'D'};
                    mtx_inputs_to_server.lock();
                    inputs_to_server.push_back(mov);
                    mtx_inputs_to_server.unlock();
                }
            }
            else if (character == KEY_LEFT) {  //Se presiono la flecha hacia la izquierda
                if (current_num_players == num_jugadores && end_game == false) {
                    std::vector<char> mov = {'M', 'L'};
                    mtx_inputs_to_server.lock();
                    inputs_to_server.push_back(mov);
                    mtx_inputs_to_server.unlock();
                }
            }
            else if (character == KEY_RIGHT) {  //Se presiono la flecha hacia la derecha
                if (current_num_players == num_jugadores && end_game == false) {
                    std::vector<char> mov = {'M', 'R'};
                    mtx_inputs_to_server.lock();
                    inputs_to_server.push_back(mov);
                    mtx_inputs_to_server.unlock();
                }
            }
            else if (character >= 32 and character <= 127) {   //Caracteres imprimibles
                current_buffer.push_back((char) character);
                waddch(window_input, (char) character);
            }
        }
        //Imprimir mensajes en el chat
        mtx_msgs_from_chat.lock();
        while (msgs_from_chat.empty() == false) {
            waddch(window_chat, msgs_from_chat.front());
            msgs_from_chat.erase(msgs_from_chat.begin());
        }
        mtx_msgs_from_chat.unlock();
        wrefresh(window_chat);
        
        //Imprimir snakes en el juego
        int x_offset_win = 1;
        int y_offset_win = 1;
        mtx_game.lock();
        for (auto it = game.ids_clients_snakes.begin(); it != game.ids_clients_snakes.end(); ++it) {
            Snake &s = it->second;
            while (s.body_erased.empty() == false) {
                PosXY tail_erased = s.body_erased.front();
                s.body_erased.pop_front();
                wmove(window_juego, y_offset_win + tail_erased.y, x_offset_win + tail_erased.x);
                waddch(window_juego, ' ');
            }
            for (auto it2 = s.body_position.begin(); it2 != s.body_position.end(); ++it2) {
                wmove(window_juego, y_offset_win + it2->y, x_offset_win + it2->x);
                waddch(window_juego, s.body_character);
            }
        }
        mtx_game.unlock();
        wrefresh(window_juego);

        //Imprimir el nickname en la parte superior izquierda de la pantalla
        move(0, 0);
        for (int i = 0; i < 80; ++i) {
            addch(' ');
        }
        move(0, 0);
        std::string user = "Nickname: ";
        printw(user.c_str());
        mtx_nickname.lock();
        printw(nickname.c_str());
        mtx_nickname.unlock();
        refresh();

        //Imprimir los nicknames y su caracter
        mtx_id_client_nickname_pieza.lock();
        int i = 1;
        for (auto it = id_client_nickname_pieza.begin(); it != id_client_nickname_pieza.end(); ++it) {
            wmove(window_users, i, 0);
            for (int i = 0; i < 20; ++i) {
                waddch(window_users, ' ');
            }
            wmove(window_users, i, 0);
            wprintw(window_users, (it->second).first.c_str());

            wmove(window_piezas, i, 0);
            waddch(window_piezas, (it->second).second);
            ++i;
        }
        mtx_id_client_nickname_pieza.unlock();
        wrefresh(window_users);
        wrefresh(window_piezas);
    }
    
    endwin();

    return 0;
}
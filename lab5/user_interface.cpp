#include "user_interface.h"

void create_windows_for_game(WINDOW **window_juego, WINDOW **window_chat, WINDOW **window_input, WINDOW **window_users, WINDOW **window_piezas) {
    int n_rows_w_juego = 35;
    int n_cols_w_juego = 80;
    int start_x_w_juego = 1;
    int start_y_w_juego = 1;
    *window_juego = newwin(n_rows_w_juego, n_cols_w_juego, start_y_w_juego, start_x_w_juego);
    box(*window_juego, 0, 0);

    int n_rows_w_chat = 31;
    int n_cols_w_chat = 68;
    int start_x_w_chat = start_x_w_juego + n_cols_w_juego;
    int start_y_w_chat = 0;
    *window_chat = newwin(n_rows_w_chat, n_cols_w_chat, start_y_w_chat, start_x_w_chat);
    //box(*window_chat, 0, 0);

    int n_rows_w_input = 5;
    int n_cols_w_input = 68;
    int start_x_w_input = start_x_w_juego + n_cols_w_juego;
    int start_y_w_input = start_y_w_chat + n_rows_w_chat;
    *window_input = newwin(n_rows_w_input, n_cols_w_input, start_y_w_input, start_x_w_input);
    //box(*window_input, 0, 0);

    int n_rows_w_users = 20;
    int n_cols_w_users = 20;
    int start_x_w_users = start_x_w_chat + n_cols_w_chat;
    int start_y_w_users = 0;
    *window_users = newwin(n_rows_w_users, n_cols_w_users, start_y_w_users, start_x_w_users);
    //box(*window_users, 0, 0);

    int n_rows_w_piezas = 20;
    int n_cols_w_piezas = 8;
    int start_x_w_piezas = start_x_w_users + n_cols_w_users;
    int start_y_w_piezas = 0;
    *window_piezas = newwin(n_rows_w_piezas, n_cols_w_piezas, start_y_w_piezas, start_x_w_piezas);
    //box(*window_piezas, 0, 0);
}
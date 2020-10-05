#include <iostream>
#include <thread>
#include <ncurses.h>
#include <chrono>

#include "go_back_n.h"
#include "process_incoming_segments.h"
#include "process_input_from_client.h"
#include "process_timer.h"

std::queue<std::vector<char> > queue_requests;

std::mutex mtx_queue_requests;
std::mutex mtx_cout;

int main()
{
    std::string my_host_ip = "190.237.34.131";
    int my_host_port = 50003;
    std::string server_host_ip = "51.15.220.108";
    int server_host_port = 60003;

    std::list<std::pair<event_type, void *> > list_events1;
    std::mutex mtx_list_events1;

    std::queue<Data> incoming_msgs1;
    std::mutex mtx_incoming_msgs1;

    int sockfd1;
    sockaddr_in server_addr;
    socklen_t len_server_addr;

    int cont_segments_timers1 = 0;
    std::chrono::steady_clock::time_point begin_timer1;
    std::mutex mtx_timer1;

    init_network_layer(my_host_ip, my_host_port, server_host_ip, server_host_port,
                       &sockfd1, &server_addr, &len_server_addr);

    std::thread(process_input_from_client, &list_events1, &mtx_list_events1).detach();
    
    std::thread(process_incoming_segments, &sockfd1, &server_addr, &len_server_addr, &list_events1, &mtx_list_events1).detach();

    std::thread(process_timer, &list_events1, &mtx_list_events1, &cont_segments_timers1, &begin_timer1, &mtx_timer1).detach();
    
    std::thread(go_back_n, &sockfd1, &server_addr, &list_events1, &mtx_list_events1, &incoming_msgs1, &mtx_incoming_msgs1, &cont_segments_timers1, &begin_timer1, &mtx_timer1).detach();

    initscr();

    WINDOW *window_requests_answers;
    WINDOW *window_input;

    int n_rows_w_req_ans = 35;
    int n_cols_w_req_ans = 80;
    int start_x_w_req_ans = 0;
    int start_y_w_req_ans = 0;
    window_requests_answers = newwin(n_rows_w_req_ans, n_cols_w_req_ans, start_y_w_req_ans, start_x_w_req_ans);
    //box(window_requests_answers, 0, 0);

    int n_rows_w_input = 31;
    int n_cols_w_input = 68;
    int start_x_w_input = start_x_w_req_ans + n_cols_w_req_ans;
    int start_y_w_input = 0;
    window_input = newwin(n_rows_w_input, n_cols_w_input, start_y_w_input, start_x_w_input);

    curs_set(0);
    noecho();
    nodelay(window_input, true);
    keypad(window_input, true);

    wprintw(window_requests_answers, "Log:");
    wprintw(window_input, "Consultas:");

    wmove(window_requests_answers, 1, 0);
    wmove(window_input, 1, 0);

    refresh();
    wrefresh(window_requests_answers);
    wrefresh(window_input);

    int character;
    std::vector<char> current_buffer;
    
    while (true)
    {
        //Leer input y procesar
        character = wgetch(window_input);

        if (character != -1) { //Se presiono un caracter
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
                //mtx_inputs_to_server.lock();
                //inputs_to_server.push_back(current_buffer);
                //mtx_inputs_to_server.unlock();

                mtx_queue_requests.lock();
                queue_requests.push(current_buffer);
                mtx_queue_requests.unlock();

                
                //for (int i = 0; i < current_buffer.size(); ++i)
                //{
                //    waddch(window_requests_answers, current_buffer[i]);
                //}
                //waddch(window_requests_answers, '\n');
                //wrefresh(window_requests_answers);

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
            else if (character >= 32 and character <= 127) {   //Caracteres imprimibles
                current_buffer.push_back((char) character);
                waddch(window_input, (char) character);
            }
        }

        //Imprimir las respuestas a las consultas realizadas
        mtx_incoming_msgs1.lock();
        if (incoming_msgs1.empty() == false)
        {
            Data incoming_msg = incoming_msgs1.front();
            incoming_msgs1.pop();
            wprintw(window_requests_answers, "Tipo = %d; ", (int) incoming_msg.type);
            wprintw(window_requests_answers, "Numero de Req/Ans = %d; ", incoming_msg.req_ans_number);
            wprintw(window_requests_answers, "Numero de Fragmento = %d; ", incoming_msg.fragment_number);
            wprintw(window_requests_answers, "Es ultimo Fragmento = %d; ", (int) incoming_msg.is_last_fragment);
            wprintw(window_requests_answers, "Numero de Bytes usados en data = %d; ", incoming_msg.bytes_used_in_data);
            wprintw(window_requests_answers, "Data = %s\n", incoming_msg.data);
            waddch(window_requests_answers, '\n');
            wrefresh(window_requests_answers);
        }
        mtx_incoming_msgs1.unlock();
    }

    endwin();

    return 0;
}
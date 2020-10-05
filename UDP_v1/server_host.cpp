#include <iostream>
#include <thread>
#include <unordered_map>

#include "go_back_n.h"
#include "process_incoming_segments.h"
#include "process_timer.h"
#include "process_req_ans.h"

std::mutex mtx_cout;

std::unordered_map<int, std::pair< std::list<std::pair<event_type, void *> >*, std::mutex *> > id_slave_list_events;

int main()
{
    std::string my_host_ip = "51.15.220.108";
    int my_host_port = 60003;
    std::string client_host_ip = "190.237.34.131";
    int client_host_port = 50003;
    std::string slave1_host_ip = "51.15.220.108";
    int slave1_host_port = 60103;
    std::string slave2_host_ip = "51.15.220.108";
    int slave2_host_port = 60203;
    std::string slave3_host_ip = "51.15.220.108";
    int slave3_host_port = 60303;
    std::string slave4_host_ip = "51.15.220.108";
    int slave4_host_port = 60403;

    std::list<std::pair<event_type, void *> > list_events1;
    std::mutex mtx_list_events1;
    std::list<std::pair<event_type, void *> > list_events2;
    std::mutex mtx_list_events2;
    std::list<std::pair<event_type, void *> > list_events3;
    std::mutex mtx_list_events3;
    std::list<std::pair<event_type, void *> > list_events4;
    std::mutex mtx_list_events4;
    std::list<std::pair<event_type, void *> > list_events5;
    std::mutex mtx_list_events5;

    id_slave_list_events[-1] = std::pair< std::list<std::pair<event_type, void *> >*, std::mutex *>(&list_events1, &mtx_list_events1);
    id_slave_list_events[0] = std::pair< std::list<std::pair<event_type, void *> >*, std::mutex *>(&list_events2, &mtx_list_events2);
    id_slave_list_events[1] = std::pair< std::list<std::pair<event_type, void *> >*, std::mutex *>(&list_events3, &mtx_list_events3);

    std::queue<Data> incoming_msgs1;
    std::mutex mtx_incoming_msgs1;
    std::queue<Data> incoming_msgs2;
    std::mutex mtx_incoming_msgs2;
    std::queue<Data> incoming_msgs3;
    std::mutex mtx_incoming_msgs3;
    std::queue<Data> incoming_msgs4;
    std::mutex mtx_incoming_msgs4;
    std::queue<Data> incoming_msgs5;
    std::mutex mtx_incoming_msgs5;

    int sockfd1, sockfd2, sockfd3, sockfd4, sockfd5;
    sockaddr_in client_addr, slave1_addr, slave2_addr, slave3_addr, slave4_addr;
    socklen_t len_client_addr, len_slave1_addr, len_slave2_addr, len_slave3_addr, len_slave4_addr;

    int cont_segments_timers1 = 0;
    std::chrono::steady_clock::time_point begin_timer1;
    std::mutex mtx_timer1;
    int cont_segments_timers2 = 0;
    std::chrono::steady_clock::time_point begin_timer2;
    std::mutex mtx_timer2;
    int cont_segments_timers3 = 0;
    std::chrono::steady_clock::time_point begin_timer3;
    std::mutex mtx_timer3;
    int cont_segments_timers4 = 0;
    std::chrono::steady_clock::time_point begin_timer4;
    std::mutex mtx_timer4;
    int cont_segments_timers5 = 0;
    std::chrono::steady_clock::time_point begin_timer5;
    std::mutex mtx_timer5;

    std::queue<Data> requests_answers1;
    std::mutex mtx_requests_answers1;
    std::queue<Data> requests_answers2;
    std::mutex mtx_requests_answers2;
    std::queue<Data> requests_answers3;
    std::mutex mtx_requests_answers3;
    std::queue<Data> requests_answers4;
    std::mutex mtx_requests_answers4;
    std::queue<Data> requests_answers5;
    std::mutex mtx_requests_answers5;

    init_network_layer(my_host_ip, my_host_port, client_host_ip, client_host_port,
                       &sockfd1, &client_addr, &len_client_addr);
    init_network_layer(my_host_ip, my_host_port, slave1_host_ip, slave1_host_port,
                       &sockfd2, &slave1_addr, &len_slave1_addr);
    init_network_layer(my_host_ip, my_host_port, slave2_host_ip, slave2_host_port,
                       &sockfd3, &slave2_addr, &len_slave2_addr);
                       /*
    init_network_layer(my_host_ip, my_host_port, slave3_host_ip, slave3_host_port,
                       &sockfd4, &slave3_addr, &len_slave3_addr);
    init_network_layer(my_host_ip, my_host_port, slave4_host_ip, slave4_host_port,
                       &sockfd5, &slave4_addr, &len_slave4_addr);*/
    
    std::thread(process_incoming_segments, &sockfd1, &client_addr, &len_client_addr, &list_events1, &mtx_list_events1).detach();
    std::thread(process_incoming_segments, &sockfd2, &slave1_addr, &len_slave1_addr, &list_events2, &mtx_list_events2).detach();
    std::thread(process_incoming_segments, &sockfd3, &slave2_addr, &len_slave2_addr, &list_events3, &mtx_list_events3).detach();
    /*
    std::thread(process_incoming_segments, &sockfd4, &slave3_addr, &len_slave3_addr, &list_events4, &mtx_list_events4).detach();
    std::thread(process_incoming_segments, &sockfd5, &slave4_addr, &len_slave4_addr, &list_events5, &mtx_list_events5).detach();*/

    std::thread(process_timer, &list_events1, &mtx_list_events1, &cont_segments_timers1, &begin_timer1, &mtx_timer1).detach();
    std::thread(process_timer, &list_events2, &mtx_list_events2, &cont_segments_timers2, &begin_timer2, &mtx_timer2).detach();
    std::thread(process_timer, &list_events3, &mtx_list_events3, &cont_segments_timers3, &begin_timer3, &mtx_timer3).detach();
    /*
    std::thread(process_timer, &list_events4, &mtx_list_events4, &cont_segments_timers4, &begin_timer4, &mtx_timer4).detach();
    std::thread(process_timer, &list_events5, &mtx_list_events5, &cont_segments_timers5, &begin_timer5, &mtx_timer5).detach();*/

    std::thread(go_back_n, &sockfd1, &client_addr, &list_events1, &mtx_list_events1, &incoming_msgs1, &mtx_incoming_msgs1, &cont_segments_timers1, &begin_timer1, &mtx_timer1).detach();
    std::thread(go_back_n, &sockfd2, &slave1_addr, &list_events2, &mtx_list_events2, &incoming_msgs2, &mtx_incoming_msgs2, &cont_segments_timers2, &begin_timer2, &mtx_timer2).detach();
    std::thread(go_back_n, &sockfd3, &slave2_addr, &list_events3, &mtx_list_events3, &incoming_msgs3, &mtx_incoming_msgs3, &cont_segments_timers3, &begin_timer3, &mtx_timer3).detach();
    /*
    std::thread(go_back_n, &sockfd4, &slave3_addr, &list_events4, &mtx_list_events4, &incoming_msgs4, &mtx_incoming_msgs4, &cont_segments_timers4, &begin_timer4, &mtx_timer4).detach();
    std::thread(go_back_n, &sockfd5, &slave4_addr, &list_events5, &mtx_list_events5, &incoming_msgs5, &mtx_incoming_msgs5, &cont_segments_timers5, &begin_timer5, &mtx_timer5).detach();*/

    std::thread(process_requests_answers, &requests_answers1, &mtx_requests_answers1).detach();
    std::thread(process_requests_answers, &requests_answers2, &mtx_requests_answers2).detach();
    std::thread(process_requests_answers, &requests_answers3, &mtx_requests_answers3).detach();
    /*
    std::thread(process_requests_answers, &requests_answers4, &mtx_requests_answers4).detach();
    std::thread(process_requests_answers, &requests_answers5, &mtx_requests_answers5).detach();*/

    mtx_cout.lock();
    std::cout << "Log:\n";
    mtx_cout.unlock();

    Data incoming_msg;
    while (true)
    {
        mtx_incoming_msgs1.lock();
        while (incoming_msgs1.empty() == false)
        {
            incoming_msg = incoming_msgs1.front();
            incoming_msgs1.pop();
            if (incoming_msg.type != 3) //No es Auxiliar
            {
                print_Data(&incoming_msg);
                mtx_cout.lock();
                std::cout << "\n";
                mtx_cout.unlock();
                if (incoming_msg.type == 0 || incoming_msg.type == 1) //Es Request o Answer
                {
                    mtx_requests_answers1.lock();
                    requests_answers1.push(incoming_msg);
                    mtx_requests_answers1.unlock();
                }
            }
        }
        mtx_incoming_msgs1.unlock();
    }

    return 0;
}
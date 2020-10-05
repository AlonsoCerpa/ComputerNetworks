#ifndef GO_BACK_N_H
#define GO_BACK_N_H

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <queue>
#include <vector>
#include <mutex>
#include <unordered_map>

#define MAX_SEQ 65535 //numero de secuencia maximo
#define WINDOW_SIZE 14
#define MAX_SEGMENT 500  //max longitud del segmento en bytes
#define MAX_DATA 481  //max longitud de los datos de Data
#define inc(k) if (k < MAX_SEQ) k = k + 1; else k = 0

typedef enum {segment_arrival, cksum_err, time_out, app_layer_ready} event_type;
typedef unsigned int seq_nr; 

class Data {
public:
    Data();

    unsigned char type;   //0 -> Request, 1 -> Answer, 2 -> ACK, 3 -> Auxiliar
    unsigned int req_ans_number;
    unsigned int fragment_number;
    unsigned char is_last_fragment;
    unsigned int bytes_used_in_data;
    unsigned char data[MAX_DATA];
};

struct Segment {  
    seq_nr sequence_number; 
    seq_nr ack_number; 
    Data data;
};

extern std::mutex mtx_cout;
std::unordered_map<int, std::pair< std::list<std::pair<event_type, void *> >*, std::mutex *> > id_slave_list_events;

void print_Data(Data *d);
int find_hash(std::vector<char> &bytes);
bool between(seq_nr a, seq_nr b, seq_nr c);
void send_segment(seq_nr segment_nr, seq_nr segment_expected, Data buffer[],
                  int *sockfd, sockaddr_in *other_addr, int *cont_segments_timers,
                  std::chrono::steady_clock::time_point *begin_timer,
                  std::mutex *mtx_timer, bool is_resend);
void go_back_n(int *sockfd, sockaddr_in *other_addr,
               std::list<std::pair<event_type, void *> > *list_events,
               std::mutex *mtx_list_events, std::queue<Data> *incoming_msgs,
               std::mutex *mtx_incoming_msgs, int *cont_segments_timers,
               std::chrono::steady_clock::time_point *begin_timer,
               std::mutex *mtx_timer);
void wait_for_event(event_type *event, void **payload,
                    std::list<std::pair<event_type, void *> > *list_events,
                    std::mutex *mtx_list_events, bool *app_layer_enabled);
void to_app_layer(Data *p, std::queue<Data> *incoming_msgs,
                  std::mutex *mtx_incoming_msgs,
                  std::list<std::pair<event_type, void *> > *list_events,
                  std::mutex *mtx_list_events);
void init_network_layer(std::string my_host_ip, int my_host_port,
                        std::string other_host_ip, int other_host_port,
                        int *sockfd, sockaddr_in *other_addr, socklen_t *len_other_addr);
void to_network_layer(Segment *s, int *sockfd, sockaddr_in *other_addr);
void start_or_sum_count_timer(int *cont_segments_timers,
                              std::chrono::steady_clock::time_point *begin_timer,
                              std::mutex *mtx_timer);
void stop_or_restart_timer(int *cont_segments_timers,
                           std::chrono::steady_clock::time_point *begin_timer,
                           std::mutex *mtx_timer);

#endif
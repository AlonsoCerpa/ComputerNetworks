#include <cstring>

#include "go_back_n.h"

 Data::Data()
 {
    this->type = 0;   //0 -> Request, 1 -> Answer, 2 -> ACK, 3 -> Auxiliar
    this->req_ans_number = 0;
    this->fragment_number = 0;
    this->is_last_fragment = 1;
    this->bytes_used_in_data = 0;
    std::memset(this->data, 0, MAX_DATA);
 }

void print_Data(Data *d)
{
    mtx_cout.lock();
    std::cout << "Tipo = " << (int) d->type << "; ";
    std::cout << "Numero de Req/Ans = " << d->req_ans_number << "; ";
    std::cout << "Numero de Fragmento = " << d->fragment_number << "; ";
    std::cout << "Es ultimo Fragmento = " << (int) d->is_last_fragment << "; ";
    std::cout << "Numero de bytes usados en data = " << (int) d->bytes_used_in_data << "; ";
    std::cout << "Data = " << d->data << "\n";
    mtx_cout.unlock();
}

int find_hash(std::vector<char> &bytes)
{
    int suma = 0;
    for (int i = 0; i < bytes.size(); ++i)
    {
        suma += bytes[i];
    }
    return suma % 77;
}

bool between(seq_nr a, seq_nr b, seq_nr c)
{
    if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
        return(true);
    else
        return(false);
}

void send_segment(seq_nr segment_nr, seq_nr segment_expected, Data buffer[],
                  int *sockfd, sockaddr_in *other_addr, int *cont_segments_timers,
                  std::chrono::steady_clock::time_point *begin_timer,
                  std::mutex *mtx_timer, bool is_resend)
{
    Segment s; 

    s.data = buffer[segment_nr]; 
    s.sequence_number = segment_nr; 
    s.ack_number = (segment_expected + MAX_SEQ) % (MAX_SEQ + 1);
    to_network_layer(&s, sockfd, other_addr);
    if (is_resend == false)
    {
        start_or_sum_count_timer(cont_segments_timers, begin_timer, mtx_timer);
    }
}

void go_back_n(int *sockfd, sockaddr_in *other_addr,
               std::list<std::pair<event_type, void *> > *list_events,
               std::mutex *mtx_list_events, std::queue<Data> *incoming_msgs,
               std::mutex *mtx_incoming_msgs, int *cont_segments_timers,
               std::chrono::steady_clock::time_point *begin_timer,
               std::mutex *mtx_timer)
{
    seq_nr next_segment_to_send; 
    seq_nr ack_expected; 
    seq_nr segment_expected; 
    Segment r;
    Data buffer[WINDOW_SIZE]; 
    seq_nr nbuffered; 
    seq_nr i; 
    event_type event;
    bool app_layer_enabled = true;
    void *payload;
    char *char_ptr;
    Data *data_ptr;

    ack_expected = 0; 
    next_segment_to_send = 0; 
    segment_expected = 0; 
    nbuffered = 0;

    while (true) {
        wait_for_event(&event, &payload, list_events, mtx_list_events, &app_layer_enabled); 
        switch(event) {
            case app_layer_ready:
                data_ptr = (Data *) payload;
                buffer[next_segment_to_send] = *data_ptr;
                delete data_ptr;
                nbuffered = nbuffered + 1; 
                send_segment(next_segment_to_send, segment_expected, buffer, sockfd, other_addr, cont_segments_timers, begin_timer, mtx_timer, false);
                inc(next_segment_to_send); 
                break;
            case segment_arrival:
                r = *((Segment *) payload);
                char_ptr = (char *) payload;
                delete[] char_ptr;
                if (r.sequence_number == segment_expected) {
                    to_app_layer(&r.data, incoming_msgs, mtx_incoming_msgs, list_events, mtx_list_events); 
                    inc(segment_expected);
                }
                while (between(ack_expected, r.ack_number, next_segment_to_send)) {
                    nbuffered = nbuffered - 1; 
                    stop_or_restart_timer(cont_segments_timers, begin_timer, mtx_timer); 
                    inc(ack_expected); 
                }
                break;
            case cksum_err:
                break; 
            case time_out:
                Data d;
                d.type = 3; //Tipo Auxiliar
                d.bytes_used_in_data = 7;
                std::string msg = "Timeout";
                std::memcpy(d.data, &msg[0], msg.size());
                mtx_incoming_msgs->lock();
                incoming_msgs->push(d);
                mtx_incoming_msgs->unlock();
                next_segment_to_send = ack_expected;
                for (i = 1; i <= nbuffered; i++) {
                    send_segment(next_segment_to_send, segment_expected, buffer, sockfd, other_addr, cont_segments_timers, begin_timer, mtx_timer, true);
                    inc(next_segment_to_send); 
                }
        }
        if (nbuffered < WINDOW_SIZE)
        {
            app_layer_enabled = true;
        }
        else
        {
            app_layer_enabled = false;
        }
    }
}

void wait_for_event(event_type *event, void **payload,
                    std::list<std::pair<event_type, void *> > *list_events,
                    std::mutex *mtx_list_events, bool *app_layer_enabled)
{
    bool found_event = false;
    std::pair<event_type, void *> event_payload;
    while (found_event == false)
    {
        mtx_list_events->lock();
        if (list_events->empty() == false)
        {
            if (*app_layer_enabled == true)
            {
                event_payload = list_events->front();
                *event = event_payload.first;
                *payload = event_payload.second;
                list_events->pop_front();
                found_event = true;
            }
            else
            {
                //Buscar evento que no sea app_layer_ready
                auto it = list_events->begin();
                while (it != list_events->end() && it->first == app_layer_ready)
                {   
                    ++it;
                }
                if (it != list_events->end())
                {
                    *event = it->first;
                    *payload = it->second;
                    list_events->erase(it);
                    found_event = true;
                }                
            }
        }
        mtx_list_events->unlock();
    }
}

void to_app_layer(Data *p, std::queue<Data> *incoming_msgs,
                  std::mutex *mtx_incoming_msgs,
                  std::list<std::pair<event_type, void *> > *list_events,
                  std::mutex *mtx_list_events)
{
    mtx_incoming_msgs->lock();
    incoming_msgs->push(*p);
    mtx_incoming_msgs->unlock();

    if (p->type != 2)  //Si no es un Segment tipo ACK
    {
        //Crear evento de app_layer_ready que contenga un ACK
        Data *data = new Data;
        data->type = 2;          //tipo ACK
        data->req_ans_number = 0;
        data->fragment_number = 0;
        data->is_last_fragment = 1;
        data->bytes_used_in_data = 0;
        std::memset(data->data, 0, MAX_DATA);

        mtx_list_events->lock();
        list_events->push_back(std::pair<event_type, void *>(app_layer_ready, (void *) data));
        mtx_list_events->unlock();
    }
}

void init_network_layer(std::string my_host_ip, int my_host_port,
                        std::string other_host_ip, int other_host_port,
                        int *sockfd, sockaddr_in *other_addr, socklen_t *len_other_addr)
{
    sockaddr_in my_addr;

    hostent *other_host = (hostent *)gethostbyname((char *)other_host_ip.c_str());  //direccion del otro host

    if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&my_addr, 0, sizeof(my_addr));
    memset(other_addr, 0, sizeof(*other_addr));

    //Llenando informacion del otro host
    other_addr->sin_family = AF_INET; // IPv4
    other_addr->sin_port = htons(other_host_port);
    other_addr->sin_addr = *((struct in_addr *)(other_host)->h_addr);

    //Llenando informacion de mi host
    my_addr.sin_family = AF_INET; // IPv4
    my_addr.sin_port = htons(my_host_port);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    //Enlazar mi socket con mi direccion ip y puerto
    if (bind(*sockfd, (const struct sockaddr *)&my_addr, sizeof(my_addr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    *len_other_addr = sizeof(*other_addr);
}

void to_network_layer(Segment *s, int *sockfd, sockaddr_in *other_addr)
{
    sendto(*sockfd, (const void *)s, MAX_SEGMENT, MSG_CONFIRM,
           (const sockaddr *)other_addr, sizeof(*other_addr));
}

void start_or_sum_count_timer(int *cont_segments_timers,
                              std::chrono::steady_clock::time_point *begin_timer,
                              std::mutex *mtx_timer)
{
    mtx_timer->lock();
    if (*cont_segments_timers == 0)
    {
        *begin_timer = std::chrono::steady_clock::now();
    }
    *cont_segments_timers = *cont_segments_timers + 1;
    mtx_timer->unlock();
}

void stop_or_restart_timer(int *cont_segments_timers,
                           std::chrono::steady_clock::time_point *begin_timer,
                           std::mutex *mtx_timer)
{
    mtx_timer->lock();
    *begin_timer = std::chrono::steady_clock::now();
    *cont_segments_timers = *cont_segments_timers - 1;
    mtx_timer->unlock();
}

#include <cstring>

#include "process_input_from_client.h"
#include "go_back_n.h"

void process_input_from_client(std::list<std::pair<event_type, void *> > *list_events, std::mutex *mtx_list_events)
{
    int request_cont = 0;
    std::vector<char> request;
    while (true)
    {
        mtx_queue_requests.lock();
        if (queue_requests.empty() == false)
        {
            request = queue_requests.front();
            queue_requests.pop();

            //Procesar solicitud y verificar si es valida
            //Si es valida, entonces crear un evento app_layer_ready junto con un objeto tipo Data

            if (request.size() >= 1)
            {
                Data *data = new Data;
                data->type = 0;
                data->req_ans_number = request_cont;
                ++request_cont;
                data->fragment_number = 0;
                data->is_last_fragment = 1;
                data->bytes_used_in_data = request.size() * sizeof(char);
                std::memset(data->data, 0, MAX_DATA);
                std::memcpy(data->data, &request[0], request.size() * sizeof(char));

                mtx_list_events->lock();
                list_events->push_back(std::pair<event_type, void *>(app_layer_ready, (void *) data));
                mtx_list_events->unlock();
            }
        }
        mtx_queue_requests.unlock();
    }
}
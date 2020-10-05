#include <cstring>

#include "process_req_ans.h"

void process_requests_answers(std::queue<Data> *requests_answers,
                              std::mutex *mtx_requests_answers)
{
    Data d;
    std::vector<std::string> words;
    std::string word;
    while (true)
    {
        mtx_requests_answers->lock();
        if (requests_answers->empty() == false)
        {
            d = requests_answers->front();
            requests_answers->pop();
            
            words = std::vector<std::string>();
            word = std::string();
            for (int i = 0; i < d.bytes_used_in_data; ++i)
            {
                if (d.data[i] != ' ')
                {
                    word.push_back(d.data[i]);
                }
                else
                {
                    words.push_back(word);
                    word = std::string();
                }
            }
            words.push_back(word);
            word = std::string();

            if (d.type = 0)  //Es tipo Request
            {
                if (words[0] == "CN") //ID de Request es Crear Nodo
                {
                    std::string name_node = words[1];
                    int num_attribs = (words.size() - 3) / 2;
                    std::vector<std::pair<std::string, std::string> > vec_attrib_value;
                    for (int i = 0; i < num_attribs; ++i)
                    {
                        vec_attrib_value.push_back(std::pair<std::string, std::string>(words[3 + i*2], words[4 + i*2]));
                    }
                    int id_slave = 0;
                    for (int i = 0; i < name_node.size(); ++i)
                    {
                        id_slave += (int) name_node[i];
                    }
                    id_slave = id_slave % 2;

                    //Enviar Request al Slave correspondiente
                    Data *data = new Data;
                    data->type = 0;
                    data->req_ans_number = d.req_ans_number;
                    data->fragment_number = 0;
                    data->is_last_fragment = 1;
                    data->bytes_used_in_data = d.bytes_used_in_data;
                    std::memcpy(data->data, d.data, MAX_DATA);

                    std::mutex *mtx_list_events = 
                    mtx_list_events->lock();
                    list_events->push_back(std::pair<event_type, void *>(app_layer_ready, (void *) data));
                    mtx_list_events->unlock();
                }
                else if (words[0] == "CL") //ID de Request es Crear Enlace
                {

                }
            }
            else if (d.type == 1)  //Es tipo Answer
            {

            }
        }
        mtx_requests_answers->unlock();
    }
}
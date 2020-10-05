#ifndef PROCESS_INPUT_FROM_CLIENT_H
#define PROCESS_INPUT_FROM_CLIENT_H

#include <vector>
#include <queue>

#include "go_back_n.h"

extern std::queue<std::vector<char> > queue_requests;

extern std::mutex mtx_queue_requests;

void process_input_from_client(std::list<std::pair<event_type, void *> > *list_events, std::mutex *mtx_list_events);

#endif
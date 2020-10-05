#ifndef PROCESS_INCOMING_SEGMENTS_H
#define PROCESS_INCOMING_SEGMENTS_H

#include "go_back_n.h"

void process_incoming_segments(int *sockfd, sockaddr_in *other_addr, socklen_t *len_other_addr, std::list<std::pair<event_type, void *> > *list_events, std::mutex *mtx_list_events);

#endif
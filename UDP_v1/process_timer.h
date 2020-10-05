#ifndef PROCESS_TIMER_H
#define PROCESS_TIMER_H

#include <chrono>
#include <mutex>
#include <list>

#include "go_back_n.h"

void process_timer(std::list<std::pair<event_type, void *> > *list_events,
                   std::mutex *mtx_list_events, int *cont_segments_timers,
                   std::chrono::steady_clock::time_point *begin_timer,
                   std::mutex *mtx_timer);

#endif
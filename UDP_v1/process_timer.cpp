#include "process_timer.h"

#include <thread>
#include <iostream>

void process_timer(std::list<std::pair<event_type, void *> > *list_events,
                   std::mutex *mtx_list_events, int *cont_segments_timers,
                   std::chrono::steady_clock::time_point *begin_timer,
                   std::mutex *mtx_timer)
{
    while (true)
    {
        mtx_timer->lock();
        if (*cont_segments_timers > 0)
        {
            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            int num_millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now - *begin_timer).count();
            if (num_millisecs > 300)
            {
                mtx_list_events->lock();
                list_events->push_back(std::pair<event_type, void *>(time_out, nullptr));
                mtx_list_events->unlock();

                *begin_timer = std::chrono::steady_clock::now();
            }
        }
        mtx_timer->unlock();
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
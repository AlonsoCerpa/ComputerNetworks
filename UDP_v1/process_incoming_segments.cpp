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

#include "process_incoming_segments.h"

void process_incoming_segments(int *sockfd, sockaddr_in *other_addr, socklen_t *len_other_addr,
                               std::list<std::pair<event_type, void *> > *list_events, std::mutex *mtx_list_events)
{
    while (true)
    {
        char *buffer = new char[MAX_SEGMENT];
        int n = recvfrom(*sockfd, buffer, MAX_SEGMENT, MSG_WAITALL,
                         (sockaddr *) other_addr, len_other_addr);

        //Revisar el checksum

        mtx_list_events->lock();
        list_events->push_back(std::pair<event_type, void *>(segment_arrival, (void *) buffer));
        mtx_list_events->unlock();
    }
}
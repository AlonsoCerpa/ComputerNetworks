#ifndef PROCESS_REQ_ANS_H
#define PROCESS_REQ_ANS_H

#include <queue>
#include <mutex>

#include "go_back_n.h"

void process_requests_answers(std::queue<Data> *requests_answers,
                              std::mutex *mtx_requests_answers);

#endif
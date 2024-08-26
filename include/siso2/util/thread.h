#ifndef SISO2_THREAD_H
#define SISO2_THREAD_H

#include <pthread.h>
#include <string>
#include <spdlog/spdlog.h>

namespace SISO2
{

static inline void setup_thread(int priority, std::string name)
{
    // spdlog::info("Starting realtime thread \"{}\" with priority {}", name, priority);
    // int policy;
    // struct sched_param param;
    // pthread_getschedparam(pthread_self(), &policy, &param);
    // param.sched_priority = priority;
    // if (pthread_setschedparam(pthread_self(), SCHED_RR, &param) != 0) { // set new policy and parameters
    //     spdlog::error("pthread_setschedparam: {}", strerror(errno));
    // }
#if __APPLE__
    pthread_setname_np(name.c_str());
#else
    pthread_setname_np(pthread_self(), name.c_str());
#endif
}

};

#endif
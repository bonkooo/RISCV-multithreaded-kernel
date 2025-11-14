#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "list.hpp"
#include "sleepingQueue.h"
#include "../lib/hw.h"

class TCB;
typedef TCB* thread_t;

class Scheduler {
private:
    static List<TCB> readyCoroutineQueue;
    static sleepingQueue sleepingThreads;
public:
    static thread_t get();
    static void put(thread_t t);
    static void waitAll();

    static int putSleeping(thread_t t, time_t time);
    static void tick();

    static List<TCB> globallyBlocked;
};

#endif

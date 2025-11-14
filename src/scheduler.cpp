#include "../h/scheduler.hpp"
#include "../h/tcb.hpp"   // now we have full TCB definition
#include "../h/sleepingQueue.h"
#include "../h/riscv.hpp"

List<TCB> Scheduler::readyCoroutineQueue;
sleepingQueue Scheduler::sleepingThreads;
List<TCB> Scheduler::globallyBlocked;

TCB* Scheduler::get() {
    return readyCoroutineQueue.removeFirst();
}

void Scheduler::put(thread_t t) {
    readyCoroutineQueue.addLast(t);
}

void Scheduler::waitAll() {
    while (Riscv::threadCount != 3) {
        thread_dispatch();
    }
}

int Scheduler::putSleeping(thread_t t, time_t time) {
    if (!t) return -1;
    t->setSleeping(true);
    sleepingThreads.add(t, time);
    return 0;
}

void Scheduler::tick() {
    sleepingThreads.passTime();
    sleepingThreads.updateQueue();
}


#include "../h/_sem.hpp"
#include "../h/tcb.hpp"
#include "../h/scheduler.hpp"

sem_t _sem::sem_open(unsigned init) {
    sem_t sem = new _sem(init);
    sem->closed = false;
    return sem;
}

int _sem::sem_close(sem_t sem) {
    if (sem == nullptr) return -1;
    sem->closed = true;
    while (thread_t t = sem->blockedThreads.removeFirst()) {
        t->setBlocked(false);
        Scheduler::put(t);
    }
    return 0;
}

int _sem::sem_wait(sem_t sem) {
    if (!sem) return -1;
    if (sem->closed) return -2;

    if (--sem->val < 0) {
        sem->blockedThreads.addLast(TCB::running);
        TCB::running->setBlocked(true);
        TCB::dispatch();
    }

    return 0;
}

int _sem::sem_signal(_sem* sem) {
    if (!sem) return -1;
    if (sem->closed) return -2;

    if (++sem->val <= 0) {
        TCB* t = sem->blockedThreads.removeFirst();
        if (t != nullptr) {
            t->setBlocked(false);
            Scheduler::put(t);
        }
    }

    return 0;
}




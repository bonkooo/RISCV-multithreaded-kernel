#ifndef PROJECT_BASE__SEM_HPP
#define PROJECT_BASE__SEM_HPP

#include "../lib/hw.h"
#include "../h/list.hpp"
#include "../h/tcb.hpp"
#include "../h/scheduler.hpp"

typedef _sem* sem_t;

class _sem {
public:
    static sem_t sem_open(unsigned init = 0);
    static int sem_close(sem_t sem);
    static int sem_wait(sem_t sem);
    static int sem_signal(sem_t sem);

    bool closed;

private:
    _sem(unsigned init = 0)
        : closed(false), val(init) {}

    int val;

    List<TCB> blockedThreads;
};

#endif //PROJECT_BASE__SEM_HPP

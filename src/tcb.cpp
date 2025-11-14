//
// Created by os on 9/24/25.
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/scheduler.hpp"

thread_t TCB::running = nullptr;
uint64 TCB::timeSliceCounter = 0;
int TCB::idCount = 9;


extern void idleWrapper(void*);
extern void userWrapper(void*);

TCB::TCB(startRoutine start_routine, void* arg)
        : start_routine(start_routine),
          arg(arg),
          stack(start_routine != nullptr ? (uint64*)MemoryAllocator::mem_alloc(DEFAULT_STACK_SIZE * sizeof(uint64)) : nullptr),
          context({
                          (uint64)&threadWrapper,
                          stack != nullptr ? (uint64)&stack[DEFAULT_STACK_SIZE] : 0
                  }),
          timeSlice(DEFAULT_TIME_SLICE),
          isMain(start_routine == nullptr),
          isSleeping(false),
          isBlocked(false),
          finished(false),
          system(false){};


void TCB::yield() {
    Riscv::writeA0(0x13);
    __asm__ volatile ("ecall");
}


void TCB::dispatch() {
    thread_t old = TCB::running;
    if (!old->isFinished() && !old->isSleeping && !old->isBlocked) Scheduler::put(old); // korutinu koja je prestala sa izvrsavanjem vrati u scheduler
    running = Scheduler::get();
    contextSwitch(&old->context, &running->context);

}

void TCB::joinThread(thread_t* handle) {
    while (!(*handle)->isFinished()) {
        TCB::dispatch();
    }
}


int TCB::thread_create(thread_t* handle, startRoutine start_routine, void* arg) {
    if (!handle || !start_routine) return -1;

    void* mem = MemoryAllocator::mem_alloc(sizeof(TCB));
    uint64* stack = (uint64*)MemoryAllocator::mem_alloc(DEFAULT_STACK_SIZE * sizeof(uint64));
    if (!stack || !mem) return -2;

    TCB* tcb = (TCB*)mem;

    tcb->start_routine = start_routine;
    tcb->arg = arg;
    tcb->stack = stack;
    tcb->context = {
        (uint64)&threadWrapper,
        (uint64)&stack[DEFAULT_STACK_SIZE]
};
    tcb->timeSlice = DEFAULT_TIME_SLICE;
    tcb->isSleeping = false;
    tcb->isBlocked = false;
    tcb->isMain = false;
    tcb->finished = false;
    tcb->system = false;

    *handle = tcb;

    if (Riscv::threadCount < Riscv::maxThreads) {
        Scheduler::put(tcb);
        Riscv::threadCount++;
    } else {
        Scheduler::globallyBlocked.addLast(tcb);
    }

    return 0;
}



thread_t TCB::createThread(Body body, void *argument) {
    thread_t t = new TCB(body, argument); t->id = 0;
    if (body != nullptr && body != userWrapper && body != idleWrapper) t->id = idCount++;
    return t;
}

int TCB::exitThread() {
    if (running->isMain) return -1;

    running->setFinished(true);

    if (Riscv::threadCount > 0) Riscv::threadCount--;

    TCB* next = Scheduler::globallyBlocked.peekFirst();
    if (next) {
        next = Scheduler::globallyBlocked.removeFirst();
        Scheduler::put(next);
        Riscv::threadCount++;
    }

    TCB* old = TCB::running;
    dispatch();

    MemoryAllocator::mem_free(old);

    return 0;
}



void TCB::createMainThread() {
    void* mem = MemoryAllocator::mem_alloc(sizeof(TCB));
    if (!mem) return;

    TCB* mainTCB = (TCB*)mem;

    mainTCB->start_routine = nullptr;
    mainTCB->arg = nullptr;
    mainTCB->stack = nullptr;
    mainTCB->timeSlice = DEFAULT_TIME_SLICE;
    mainTCB->finished = false;
    mainTCB->system = false;

    mainTCB->context = { 0, 0 };

    TCB::running = mainTCB;
}

void TCB::threadWrapper() {
    Riscv::popSppSpie();
    running->start_routine(running->arg);
    thread_exit();
}

void TCB::outputWrapper(void *arg) {
    while (true) {
        while (*((char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT &&
               Riscv::putcBuffer->getCount() > 0) {
            char c = Riscv::putcBuffer->getc();
            *((char*)CONSOLE_TX_DATA) = c;
        }
        thread_dispatch();
    }
}



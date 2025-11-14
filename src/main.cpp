#include "../h/MemAlloc.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/tcb.hpp"
#include "../lib/hw.h"
#include "../h/BoundedBuffer.hpp"
#include "../h/print.hpp"
//class TCB;
//typedef TCB* thread_t;
//class _sem;
//typedef _sem* sem_t;

class Scheduler;
extern void userMain();

void userWrapper(void*) {
    userMain();
}

void idleWrapper(void*){
    while (true) {
        thread_dispatch();
    }
}


int main() {
    MemoryAllocator::initialize();
    Riscv::wStvec((uint64)&Riscv::supervisorTrap);

    Riscv::getcBuffer = new BoundedBuffer(1024);
    Riscv::putcBuffer = new BoundedBuffer(1024);

    Riscv::msSstatus(Riscv::SSTATUS_SPP);
    TCB::createMainThread();

    Riscv::uMode = true;
    thread_t userThread, outputThread, idleThread;
    thread_create(&userThread, userWrapper, nullptr);
    thread_create(&outputThread, TCB::outputWrapper, nullptr);
    thread_create(&idleThread, idleWrapper, nullptr);

    Riscv::msSstatus(Riscv::SSTATUS_SIE);

    while (!userThread->isFinished()) {
        thread_dispatch();
    }

    while (Riscv::putcBuffer && Riscv::putcBuffer->getCount() > 0) {
        thread_dispatch();
    }

    Riscv::endEmulator();
    return 0;
}


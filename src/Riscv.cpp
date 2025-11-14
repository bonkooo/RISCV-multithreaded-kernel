//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../h/syscall_c.h"
#include "../h/print.hpp"
#include "../h/MemAlloc.hpp"
#include "../h/_sem.hpp"
#include "../test/printing.hpp"
#include "../h/syscall_cpp.hpp"

bool Riscv::uMode = false;
int Riscv::threadCount = 0;
int Riscv::maxThreads = 10;

void Riscv::popSppSpie()
{
    uMode ? mcSstatus(SSTATUS_SPP) : msSstatus(SSTATUS_SPP);
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSyscall(uint64 opCode, uint64 arg1, uint64 arg2, uint64 arg3) {
    uint64 ret;
    switch (opCode) {
        case MEM_ALLOC: {
            ret = (uint64)MemoryAllocator::mem_alloc(arg1);
            wSscratch(ret);
            break;
        }
        case MEM_FREE: {
            void* ptr = (void*)arg1;
            ret = MemoryAllocator::mem_free(ptr);
            wSscratch(ret);
            break;
        }
        case MEM_GET_FREE_SPACE: {
            size_t ret = MemoryAllocator::getFreeSpace();
            wSscratch(ret);
            break;
        }

        case MEM_GET_LARGEST_FREE_BLOCK: {
            size_t ret = MemoryAllocator::getLargestFreeBlock();
            wSscratch(ret);
            break;
        };

        case THREAD_CREATE: {
            ret = 0;
            thread_t* retThread = (thread_t*)arg1;
            *retThread = TCB::createThread((void (*)(void *))arg2, (void*)arg3);
            if (*retThread == nullptr) {
                ret = -1;
            } else {
                // odlučujemo da li nova nit postaje aktivna ili se blokira
                if (Riscv::threadCount < Riscv::maxThreads) {
                    Scheduler::put(*retThread);
                    Riscv::threadCount++;   // broj aktivnih (ready/running) se povecava
                } else {
                    Scheduler::globallyBlocked.addLast(*retThread);
                    // ne povecavamo threadCount jer nit jos nije aktivna
                }
            }
            wSscratch(ret);
            break;
        }


        case THREAD_EXIT: {
            ret = TCB::exitThread();
            threadCount--;
            wSscratch(ret);
            break;
        };

        case THREAD_DISPATCH: {
            TCB::dispatch();
            break;
        };

        case THREAD_JOIN: {
            thread_t* handle = (thread_t*) arg1;
            TCB::joinThread(handle);
            break;
        }

        case 0x15: {
            ret = TCB::running->getId();
            TCB::dispatch();
            wSscratch(ret);
            break;
        }

        case SEM_OPEN: {
            sem_t* handle = (sem_t*)arg1;
            unsigned init = (unsigned)arg2;
            *handle = _sem::sem_open(init);  // create semaphore
            wSscratch(*handle != nullptr ? 0 : -1); // 0 = success, -1 = error
            break;
        }

        case SEM_CLOSE: {
            sem_t handle = (sem_t)arg1;
            int ret = _sem::sem_close(handle); // close using new API
            wSscratch(ret);
            break;
        }

        case SEM_WAIT: {
            sem_t handle = (sem_t)arg1;
            int ret = _sem::sem_wait(handle); // wait using new API
            wSscratch(ret);
            break;
        }

        case SEM_SIGNAL: {
            sem_t handle = (sem_t)arg1;
            int ret = _sem::sem_signal(handle); // signal using new API
            wSscratch(ret);
            break;
        }


        case TIME_SLEEP: {
            ret = Scheduler::putSleeping(TCB::running, (time_t)arg1);
            TCB::dispatch();
            wSscratch(ret);
            break;
        };

        case GETC: {
            ret = getcBuffer->getc();
            wSscratch(ret);
            break;
        };

        case PUTC: {
            char c = (char)arg1;
            putcBuffer->putc(c);
            break;

        };

        default:
            _printString("UnknownSyscall scause = ");
            printHex(rScause()); newLine();
            break;
    }
}

void Riscv::handleSupervisorTrap()
{

    uint64 scause = rScause();
    uint64 sepc = rSepc();
    uint64 sstatus = rSstatus();
    uint64 a1 = readA1();
    uint64 a2 = readA2();
    uint64 a3 = readA3();
    uint64 opCode = readA0();

    switch (scause) {
        case 0x0000000000000008UL: // 08
        case 0x0000000000000009UL: // 09
            handleSyscall(opCode, a1, a2, a3);
            sepc += 4;

            break;
        case TIMER_INTERRUPT: // 11
            mcSip(SIP_SSIP);
            Scheduler::tick();

            TCB::timeSliceCounter++;
          if (TCB::running && TCB::timeSliceCounter >= TCB::running->getTimeSlice() && !TCB::running->isSleeping && !TCB::running->isBlocked)
            {
                TCB::timeSliceCounter = 0;
                TCB::dispatch();
            }

            break;
        case CONSOLE_INTERRUPT: { // 19
            uint64 irq = plic_claim();
            if (irq == CONSOLE_IRQ) {
                while (*((char *) CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT) {
                    char c = *((char *) CONSOLE_RX_DATA);
                    if (c != '\0') getcBuffer->putc(c);

                }
            }
            if (irq) plic_complete(irq);  // uvek acknowledge
            break;
        }
        default:
            _printString("trap entry scause=");
            printHex(scause);
            _printString(" sepc=");
            printHex(sepc);
            _printString(" opCode =");
            _printInteger(opCode);
            _printString(" arg1 =");
            _printInteger(a1);
            _printString("\n");
            while ((*((char *) CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT) && !((Riscv::putcBuffer->getCount() == 0))) {
                char data = Riscv::putcBuffer->getc();
                *(char *) CONSOLE_TX_DATA = data;
            }
            Riscv::endEmulator();
            break;
    }
    wSstatus(sstatus);
    wSepc(sepc);
}

BoundedBuffer* Riscv::putcBuffer = nullptr;
BoundedBuffer* Riscv::getcBuffer = nullptr;


void Riscv::endEmulator() {
    __asm__ volatile("li a1, 0x100000");
    __asm__ volatile("li a0, 0x5555");
    __asm__ volatile("sw a0, 0(a1)");
}





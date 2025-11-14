#include "../lib/mem.h"
#include "../h/syscall_cpp.hpp"
#include "../h/MemAlloc.hpp"
#include "../h/syscall_c.h"
#include "../h/riscv.hpp"

void Thread::threadWrapper(void* arg) {
    Thread* t = (Thread*)arg;
    if (t->body) t->body(t->arg);
    else t->run();
}


Thread::Thread(void (*body)(void *), void *arg) : myHandle(nullptr), body(body), arg(arg) {
    thread_create(&myHandle, threadWrapper, this);
}

Thread::~Thread() {
    if (myHandle) MemoryAllocator::mem_free(myHandle);
}

int Thread::start() {
    if (myHandle) return 0;
    return thread_create(&myHandle, threadWrapper, this);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t t) {
    return time_sleep(t);
}

void Thread::join() {
    thread_join(&myHandle);
}

void Thread::getId() {
    _printInteger(thread_id());
}


Thread::Thread() : myHandle(nullptr), body(nullptr), arg(nullptr) {}

void Thread::run() {

}

Semaphore::Semaphore(unsigned int init) {
    myHandle = (sem_t*)mem_alloc(sizeof(sem_t));
    sem_open(myHandle, init);
}

Semaphore::~Semaphore() {
    if (myHandle) {
        sem_close(*myHandle);
        mem_free(myHandle);
    }
}

int Semaphore::wait() {
    if (!myHandle || !*myHandle) return -1;
    return sem_wait(*myHandle);
}

int Semaphore::signal() {
    if (!myHandle || !*myHandle) return -1;
    return sem_signal(*myHandle);
}

void PeriodicThread::terminate() {
    thread_exit();
}

PeriodicThread::PeriodicThread(time_t period) : Thread(), period(period) {
    start();
}

void PeriodicThread::run() {
    while (true) {
        periodicActivation();
        Thread::sleep(period);
    }
}

void PeriodicThread::periodicActivation() {

}


char Console::getc() {
    if (!Riscv::getcBuffer) return (char)EOF;
    return ::getc();
}

void Console::putc(char c) {
    if (c == 127 || c == 13) return;
    if (!Riscv::putcBuffer) return;
    ::putc(c);
}


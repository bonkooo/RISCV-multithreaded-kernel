#ifndef SYSCALL_CPP_HPP
#define SYSCALL_CPP_HPP

#include "syscall_c.h"

class Thread{
public:
    Thread(void(*body)(void*), void* arg);
    virtual ~Thread();

    int start();

    static void dispatch();
    static int sleep(time_t);
    void join();
    void getId();

protected:
    Thread();
    virtual void run();

private:
    thread_t myHandle;
    void(*body)(void*); void* arg;

    static void threadWrapper(void* arg);
};

class Semaphore {
public:
    explicit Semaphore (unsigned init = 1);
    virtual ~Semaphore();

    int wait();
    int signal();

private:
    sem_t* myHandle;
};

class PeriodicThread : public Thread {
public:
    void terminate();
protected:
    explicit PeriodicThread(time_t period);
    virtual void periodicActivation();
    void run() override;
private:
    time_t period;
};

const int EOF = -1;
class Console {
public:
    static char getc();
    static void putc(char);
};

#endif
//
// Created by os on 9/24/25.
//
#ifndef PROJECT_BASE_CCB_H
#define PROJECT_BASE_CCB_H
#include "../lib/hw.h"
#include "../h/scheduler.hpp"
#include "../h/MemAlloc.hpp"
#include "syscall_cpp.hpp"

typedef TCB* thread_t;
using Body = void (*)(void *);

class TCB {
public:
    ~TCB() { if (stack) MemoryAllocator::mem_free(stack); }

    using startRoutine = void(*)(void*);

    bool isFinished() const {
        return finished;
    }

    void setFinished(bool finished) {
        this->finished = finished;
    }

    uint64 getTimeSlice() const {
        return timeSlice;
    }

    void setTimeSlice(uint64 timeSlice){
        this->timeSlice = timeSlice;
    }

    bool getSleeping(){
        return isSleeping;
    }

    void setSleeping(bool b){
        isSleeping = b;
    }

    bool getBlocked(){
        return isBlocked;
    }

    void setBlocked(bool b){
        isBlocked = b;
    }

    static void joinThread(thread_t* handle);
    static int thread_create(thread_t* handle, void(*start_routine) (void*), void* arg);
    static thread_t createThread(Body body, void* argument);
    static int exitThread();
    static void createMainThread();
    static void yield();

    static TCB* running; // trenutno izvrsavana korutina

    explicit TCB(startRoutine start_routine, void* arg);

    static void outputWrapper(void* arg);

    int getId() {return this->id;}
private:


    struct Context {
        uint64 ra;
        uint64 sp;
    };

    startRoutine start_routine;
    void* arg;

    uint64* stack;
    Context context;
    uint64 timeSlice; // broj perioda koje dobija nit
    bool isMain;
    bool isSleeping;
    bool isBlocked;
    bool finished;
    bool system;
    int id;
    static int idCount;

    friend class Riscv;
    friend class _sem;
    friend class sleepingQueue;

    // omotacka funkcija koja se izvrsava za novonapravljene niti
    static void threadWrapper();

    static uint64 timeSliceCounter;

    static void dispatch();

    static void contextSwitch(Context* oldContext, Context* newContext);

};



#endif // PROJECT_BASE_CCB_H

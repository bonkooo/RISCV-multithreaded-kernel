//
// Created by os on 9/29/25.
//
#ifndef PROJECT_BASE_SLEEPINGQUEUE_H
#define PROJECT_BASE_SLEEPINGQUEUE_H

#include "../lib/hw.h"
#include "../h/MemAlloc.hpp"

typedef unsigned long time_t;
class TCB;
typedef TCB* thread_t;
class Scheduler;

class sleepingQueue {
private:
    struct Node {
        thread_t thread;
        Node* next;
        Node* prev;
        time_t timeRelative;

        Node(thread_t thread, time_t time, Node* prev = nullptr, Node* next = nullptr)
                : thread(thread), next(next), prev(prev), timeRelative(time) {}
    };

    Node* head;
    Node* tail;

public:
    sleepingQueue() : head(nullptr), tail(nullptr) {}

    Node* getFirst() { return head; }

    time_t getFirstTimeSlice() {
        return head ? head->timeRelative : 0;
    }

    void passTime();

    void updateQueue();

    void add(thread_t thread, time_t time);
};

#endif //PROJECT_BASE_SLEEPINGQUEUE_H

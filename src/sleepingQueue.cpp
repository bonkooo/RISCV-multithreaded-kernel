#include "../h/sleepingQueue.h"
#include "../h/tcb.hpp"
#include "../h/scheduler.hpp"

void sleepingQueue::passTime() {
    if (head && head->timeRelative > 0) {
        head->timeRelative--;
        if (head->timeRelative < 0) head->timeRelative = 0;
    }
}

// ubaci threadove koji su zavrsili sleep nazad u scheduelr
void sleepingQueue::updateQueue() {
    while (head && head->timeRelative <= 0) {
        Node* node = head;
        head = head->next;
        if (head) head->prev = nullptr;
        else tail = nullptr;

        thread_t thread = node->thread;
        thread->setSleeping(false);
        Scheduler::put(thread);

        MemoryAllocator::mem_free(node);
    }
}

void sleepingQueue::add(thread_t thread, time_t time) {
    if (!head) {
        head = tail = new Node(thread, time);
        return;
    }

    Node* curr = head;
    while (curr && time >= curr->timeRelative) {
        time -= curr->timeRelative;
        curr = curr->next;
    }

    Node* newNode = new Node(thread, time);

    if (!curr) {
        // na kraju liste
        newNode->prev = tail;
        tail->next = newNode;
        tail = newNode;
    } else if (!curr->prev) {
        // na pocetku liste
        newNode->next = curr;
        curr->prev = newNode;
        head = newNode;
        curr->timeRelative -= time;
    } else {
        // u sredini liste
        newNode->prev = curr->prev;
        newNode->next = curr;
        curr->prev->next = newNode;
        curr->prev = newNode;
        curr->timeRelative -= time;
    }
}

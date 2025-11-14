//
// Created by os on 10/1/25.
//
#ifndef PROJECT_BASE_BUFFER_HPP
#define PROJECT_BASE_BUFFER_HPP

#include "../h/_sem.hpp"
#include "../lib/hw.h"

class BoundedBuffer {
public:
    explicit BoundedBuffer(uint64 size = 256);
    ~BoundedBuffer();

    void putc(char c);
    char getc();
    int tryPutc(char c);

    uint64 getCount() const { return size; }
    uint64 getCap() const { return cap; }
    bool isEmpty() const { return size == 0; }
    bool isFull() const { return size == cap; }

private:
    char* buffer;
    uint64 cap;
    uint64 head;
    uint64 tail;
    uint64 size;

    sem_t availableItems;
    sem_t availableSlots;
};

#endif // PROJECT_BASE_BUFFER_HPP

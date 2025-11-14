#include "../h/BoundedBuffer.hpp"
#include "../h/MemAlloc.hpp"
#include "../h/_sem.hpp"
#include "../h/riscv.hpp"

BoundedBuffer::BoundedBuffer(uint64 size)
        : buffer(nullptr), cap(size), head(0), tail(0), size(0),
          availableItems(nullptr), availableSlots(nullptr) {
    buffer = (char*)MemoryAllocator::mem_alloc(sizeof(char) * cap);

    availableItems = _sem::sem_open(0);
    availableSlots = _sem::sem_open(cap);
}

BoundedBuffer::~BoundedBuffer() {
    if (availableItems) _sem::sem_close(availableItems);
    if (availableSlots) _sem::sem_close(availableSlots);
    MemoryAllocator::mem_free(buffer);
}

void BoundedBuffer::putc(char c) {
    _sem::sem_wait(availableSlots);

    buffer[tail] = c;
    tail = (tail + 1) % cap;
    size++;

    _sem::sem_signal(availableItems);
}

char BoundedBuffer::getc() {
    _sem::sem_wait(availableItems);

    char c = buffer[head];
    head = (head + 1) % cap;
    size--;

    _sem::sem_signal(availableSlots);
    return c;
}

int BoundedBuffer::tryPutc(char c) {
    if (isFull()) return -1;

    buffer[tail] = c;
    tail = (tail + 1) % cap;
    size++;

    _sem::sem_signal(availableItems);
    return 0;
}

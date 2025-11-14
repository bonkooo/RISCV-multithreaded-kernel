#ifndef PROJECT_BASE_MEMALLOC_HPP
#define PROJECT_BASE_MEMALLOC_HPP

#include "../lib/hw.h"
#include "../h/print.hpp"

struct memSeg{
    memSeg* left;
    memSeg* right;
    size_t segSize;
};

class MemoryAllocator {
public:

    static memSeg* freeSegments;
    static memSeg* usedSegments;

    static void* mem_alloc(size_t size);
    static int mem_free(void* ptr);
    static size_t getFreeSpace();
    static size_t getLargestFreeBlock();

    static void initialize() {
        freeSegments = (memSeg*)HEAP_START_ADDR;
        freeSegments->segSize = (size_t)HEAP_END_ADDR - (size_t)HEAP_START_ADDR - HEADER_SIZE;
        freeSegments->left = nullptr;
        freeSegments->right = nullptr;
        usedSegments = nullptr;
    }

private:
    static size_t getPaddedSize(size_t size);
    static void updateUsed(memSeg* curr);
    static void removeFromUsed(memSeg* curr);
    static void addToFree(memSeg* newSeg);
    static void merge(memSeg* curr);
    static void createNewFragment(memSeg* curr, size_t size);
    static memSeg* findBestFit(size_t size);
    static void allocateSegment(memSeg* best, size_t size);
    static void splitSegment(memSeg* seg, size_t size);
    static void detachFreeSegment(memSeg* seg);

    static bool ptrOutOfBounds(void* ptr){
        return ptr < HEAP_START_ADDR || ptr > HEAP_END_ADDR;
    }

    static size_t constexpr HEADER_SIZE = sizeof(memSeg);
};


#endif //PROJECT_BASE_MEMALLOC_HPP

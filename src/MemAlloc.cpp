#include "../h/MemAlloc.hpp"
#include "../lib/hw.h"
#include "../h/print.hpp"

memSeg* MemoryAllocator::usedSegments = nullptr;
memSeg* MemoryAllocator::freeSegments = nullptr;


void* MemoryAllocator::mem_alloc(size_t size) {
    size = getPaddedSize(size);
    if (!freeSegments) return nullptr;

    memSeg* addr = findBestFit(size);
    if (!addr) return nullptr;

    allocateSegment(addr, size);
    return (char*)addr + HEADER_SIZE;
}

int MemoryAllocator::mem_free(void* ptr) {
    if (!ptr || ptrOutOfBounds(ptr) || !usedSegments) return -1;
    memSeg* curr = (memSeg*)((char*)ptr - HEADER_SIZE);
    removeFromUsed(curr);
    addToFree(curr);
    return 0;
}

memSeg* MemoryAllocator::findBestFit(size_t size) {
    memSeg* best = nullptr;
    for (memSeg* curr = freeSegments; curr; curr = curr->right)
        if (curr->segSize >= size && (best == nullptr || (curr->segSize < best->segSize))) {
            best = curr;
        }
    return best;
}

void MemoryAllocator::allocateSegment(memSeg* curr, size_t size) {
    if (curr->segSize > size) {
        splitSegment(curr, size);
    } else {
        detachFreeSegment(curr);
    }
    updateUsed(curr);
}

void MemoryAllocator::detachFreeSegment(memSeg* seg) {
    if (seg->left) {
        seg->left->right = seg->right;
    } else {
        freeSegments = seg->right;
    }

    if (seg->right) seg->right->left = seg->left;
    seg->left = seg->right = nullptr;
}

void MemoryAllocator::splitSegment(memSeg* seg, size_t size) {
    memSeg* newSeg = (memSeg*)((char*)seg + HEADER_SIZE + size);
    newSeg->segSize = seg->segSize - size - HEADER_SIZE;
    newSeg->left = seg->left;
    newSeg->right = seg->right;
    if (seg->left) seg->left->right = newSeg;
    else freeSegments = newSeg;
    if (seg->right) seg->right->left = newSeg;
    seg->segSize = size;
    seg->left = seg->right = nullptr;
}

size_t MemoryAllocator::getPaddedSize(size_t size) {
    return (size + MEM_BLOCK_SIZE - 1) & ~(MEM_BLOCK_SIZE - 1);
}

void MemoryAllocator::updateUsed(memSeg* curr) {
    if (!usedSegments) {
        usedSegments = curr;
        curr->left = curr->right = nullptr;
        return;
    }

    if ((char*)curr < (char*)usedSegments) {
        curr->right = usedSegments;
        curr->left = nullptr;
        usedSegments->left = curr;
        usedSegments = curr;
        return;
    }

    memSeg* temp = usedSegments;
    while (temp->right && (char*)curr > (char*)temp->right) temp = temp->right;
    curr->right = temp->right;
    curr->left = temp;
    temp->right = curr;
    if (curr->right) curr->right->left = curr;
}

void MemoryAllocator::removeFromUsed(memSeg* curr) {
    if (curr->left) curr->left->right = curr->right;
    else usedSegments = curr->right;
    if (curr->right) curr->right->left = curr->left;
    curr->left = curr->right = nullptr;
}

void MemoryAllocator::addToFree(memSeg* seg) {
    if (!freeSegments) {
        freeSegments = seg;
        seg->left = seg->right = nullptr;
        return;
    }

    if ((char*)seg < (char*)freeSegments) {
        seg->right = freeSegments;
        seg->left = nullptr;
        freeSegments->left = seg;
        freeSegments = seg;
        merge(seg);
        return;
    }

    memSeg* temp = freeSegments;
    while (temp->right && (char*)temp->right < (char*)seg) temp = temp->right;
    seg->right = temp->right;
    seg->left = temp;
    temp->right = seg;
    if (seg->right) seg->right->left = seg;
    merge(seg);
}

void MemoryAllocator::merge(memSeg* curr) {
    if (curr->left && (char*)curr->left + curr->left->segSize + HEADER_SIZE == (char*)curr) {
        curr->left->segSize += curr->segSize + HEADER_SIZE;
        curr->left->right = curr->right;
        if (curr->right) curr->right->left = curr->left;
        curr = curr->left;
    }

    if (curr->right && (char*)curr + curr->segSize + HEADER_SIZE == (char*)curr->right) {
        curr->segSize += curr->right->segSize + HEADER_SIZE;
        curr->right = curr->right->right;
        if (curr->right) curr->right->left = curr;
    }
}

size_t MemoryAllocator::getFreeSpace() {
    size_t total = 0;
    for (memSeg* curr = freeSegments; curr; curr = curr->right) total += curr->segSize;
    return total;
}

size_t MemoryAllocator::getLargestFreeBlock() {
    size_t largest = 0;
    for (memSeg* curr = freeSegments; curr; curr = curr->right)
        if (curr->segSize > largest) largest = curr->segSize;
    return largest;
}

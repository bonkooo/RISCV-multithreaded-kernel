#include "../lib/mem.h"
#include "../h/syscall_cpp.hpp"
#include "../h/MemAlloc.hpp"

void *operator new(uint64 n){
    return mem_alloc(n);
}

void *operator new[](uint64 n){
    return mem_alloc(n);
}
void operator delete(void *p) noexcept {
    mem_free(p);
}

void operator delete[](void *p) noexcept {
    mem_free(p);
}

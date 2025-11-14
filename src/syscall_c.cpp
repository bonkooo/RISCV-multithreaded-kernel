#include "../h/syscall_c.h"
#include "../h/riscv.hpp"
#include "../h/print.hpp"
#include "../h/tcb.hpp"

static constexpr uint64 MEM_ALLOC = 0x01;
static constexpr uint64 MEM_FREE  = 0x02;
static constexpr uint64 MEM_GET_FREE_SPACE = 0x03;
static constexpr uint64 MEM_GET_LARGEST_FREE_BLOCK = 0x04;
static constexpr uint64 THREAD_CREATE = 0x11;
static constexpr uint64 THREAD_EXIT = 0x12;
static constexpr uint64 THREAD_DISPATCH = 0x13;
static constexpr uint64 THREAD_JOIN = 0x14;
static constexpr uint64 SEM_OPEN = 0x21;
static constexpr uint64 SEM_CLOSE = 0x22;
static constexpr uint64 SEM_WAIT = 0x23;
static constexpr uint64 SEM_SIGNAL = 0x24;
static constexpr uint64 TIME_SLEEP = 0x31;
static constexpr uint64 GETC = 0x41;
static constexpr uint64 PUTC = 0x42;

void* mem_alloc(size_t size){
    __asm__ volatile ("mv a1, %0" :: "r"(size));
    Riscv::writeA0(MEM_ALLOC);
    __asm__ volatile ("ecall");
    return (void*)Riscv::readA0();

}

int mem_free(void* ptr){
    __asm__ volatile ("mv a1, %0" :: "r"(ptr)); // ovaj pointer postane null ovde
    Riscv::writeA0(MEM_FREE);
    __asm__ volatile ("ecall");
    return (int)Riscv::readA0();

}

size_t mem_get_free_space(){
    Riscv::writeA0(MEM_GET_FREE_SPACE);
    __asm__ volatile("ecall");
    return (size_t)Riscv::readA0();
}

size_t mem_get_largest_free_block(){
    Riscv::writeA0(MEM_GET_LARGEST_FREE_BLOCK);
    __asm__ volatile("ecall");
    return (size_t)Riscv::readA0();
}

int thread_create(thread_t* handle, void(*start_routine) (void*), void* arg){
    __asm__ volatile ("mv a3, %0" :: "r"(arg));
    __asm__ volatile ("mv a2, %0" :: "r"(start_routine));
    __asm__ volatile ("mv a1, %0" :: "r"(handle));
    Riscv::writeA0(THREAD_CREATE);
    __asm__ volatile("ecall");

    return (int)Riscv::readA0(); // vraca se kod

}

int thread_exit(){
    Riscv::writeA0(THREAD_EXIT);
    __asm__ volatile("ecall");
    return (int)Riscv::readA0();
}

void thread_dispatch() {
    Riscv::writeA0(THREAD_DISPATCH);
    __asm__ volatile ("ecall");
}

void thread_join(thread_t* handle) {
    __asm__ volatile ("mv a1, %0" :: "r"(handle));
    Riscv::writeA0(THREAD_JOIN);
    __asm__ volatile ("ecall");
}

pid_t thread_id() {
    Riscv::writeA0(0x15);
    __asm__ volatile ("ecall");
    return (pid_t)Riscv::readA0();
}

int sem_open(sem_t* handle, unsigned init){
    __asm__ volatile ("mv a2, %0" :: "r"(init));
    __asm__ volatile ("mv a1, %0" :: "r"(handle));
    Riscv::writeA0(SEM_OPEN);
    __asm__ volatile ("ecall");
    return (int)Riscv::readA0();
}

int sem_close(sem_t handle){
    __asm__ volatile ("mv a1, %0" :: "r"(handle));
    Riscv::writeA0(SEM_CLOSE);
    __asm__ volatile ("ecall");
    return (int)Riscv::readA0();
}

int sem_wait(sem_t id){
    __asm__ volatile ("mv a1, %0" :: "r"(id));
    Riscv::writeA0(SEM_WAIT);
    __asm__ volatile ("ecall");
    return (int)Riscv::readA0();
}

int sem_signal(sem_t id){
    __asm__ volatile ("mv a1, %0" :: "r"(id));
    Riscv::writeA0(SEM_SIGNAL);
    __asm__ volatile ("ecall");
    return (int)Riscv::readA0();
}

int time_sleep(time_t time){
    __asm__ volatile ("mv a1, %0" :: "r"(time));
    Riscv::writeA0(TIME_SLEEP);
    __asm__ volatile ("ecall");
    return (int)Riscv::readA0();
}

char getc() {
    Riscv::writeA0(GETC);
    __asm__ volatile("ecall");
    char c = Riscv::readA0();
    return c;
}

void putc(char c) {
    __asm__ volatile("mv a1, %0" :: "r"(c));
    Riscv::writeA0(PUTC);
    __asm__ volatile("ecall");
}


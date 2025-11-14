#ifndef SYSCALL_C_H
#define SYSCALL_C_H

#include "../lib/hw.h"

class TCB;
class _sem;
typedef TCB* thread_t;
typedef _sem* sem_t;
typedef int pid_t;

void* mem_alloc(size_t size);
int mem_free(void* p);
size_t mem_get_free_space();
size_t mem_get_largest_free_block();

int thread_create(thread_t* handle, void(*start_routine) (void*), void* arg);
int thread_exit();
void thread_dispatch();
void thread_join(thread_t* handle);
pid_t thread_id();
void wait_all();

int sem_open(sem_t* handle, unsigned init);
int sem_close(sem_t handle);
int sem_wait(sem_t id);
int sem_signal(sem_t id);

int time_sleep(time_t slices);

char getc();
void putc(char c);


#endif

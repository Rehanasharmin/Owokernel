#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct Process;

typedef enum {
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_BLOCKED,
    THREAD_FINISHED
} thread_state_t;

typedef struct Thread {
    uint64_t rsp;
    uint64_t stack_base;
    uint32_t id;
    thread_state_t state;
    struct Process* process;
    struct Thread* next;
} Thread;

void thread_init(void);
Thread* thread_create(void (*entry)(void));
void thread_yield(void);

#endif

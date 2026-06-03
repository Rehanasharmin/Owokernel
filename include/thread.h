#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_FINISHED
} thread_state_t;

typedef struct Thread {
    uint64_t rsp;               // Stack pointer
    uint64_t stack_base;        // Base of the stack for cleanup
    uint32_t id;                // Unique thread ID
    thread_state_t state;       // Current state
    struct Thread* next;        // Next thread in list
} Thread;

void thread_init();
Thread* thread_create(void (*entry)());
void thread_yield();

#endif

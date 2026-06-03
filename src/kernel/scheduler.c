#include "scheduler.h"
#include "process.h"
#include "kernel.h"

static Thread* current_thread = NULL;
static Thread* thread_list_head = NULL;
static Thread* thread_list_tail = NULL;

void scheduler_init() {
    current_thread = NULL;
    thread_list_head = NULL;
    thread_list_tail = NULL;
}

void scheduler_add_thread(Thread* thread) {
    if (!thread_list_head) {
        thread_list_head = thread;
        thread_list_tail = thread;
    } else {
        thread_list_tail->next = thread;
        thread_list_tail = thread;
    }
}

Thread* scheduler_get_current() {
    return current_thread;
}

void scheduler_set_current(Thread* thread) {
    current_thread = thread;
}

extern void switch_context(Thread* old, Thread* new);

void scheduler_switch() {
    if (!current_thread) {
        current_thread = thread_list_head;
        switch_context(NULL, current_thread);
        return;
    }

    Thread* old = current_thread;
    Thread* next = old->next;
    if (!next) next = thread_list_head;

    /* 
     * Process Isolation: 
     * We need to find the Process that owns this thread and switch the PML4.
     */
    Process* next_proc = (Process*)((uintptr_t)next & ~0xFFF); // Rough cast
    if (next_proc && next_proc->pml4) {
        __asm__ volatile("mov %0, %%cr3" : : "r"(next_proc->pml4));
    }

    current_thread = next;
    switch_context(old, next);
}

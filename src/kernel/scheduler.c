#include "scheduler.h"
#include "process.h"
#include "vmm.h"
#include "kernel.h"

static Thread* current_thread = NULL;
static Thread* thread_list_head = NULL;
static Thread* thread_list_tail = NULL;

void scheduler_init(void) {
    current_thread = NULL;
    thread_list_head = NULL;
    thread_list_tail = NULL;
}

void scheduler_add_thread(Thread* thread) {
    thread->next = NULL;
    if (!thread_list_head) {
        thread_list_head = thread;
        thread_list_tail = thread;
    } else {
        thread_list_tail->next = thread;
        thread_list_tail = thread;
    }
}

Thread* scheduler_get_current(void) {
    return current_thread;
}

void scheduler_set_current(Thread* thread) {
    current_thread = thread;
}

extern void switch_context(Thread* old, Thread* new);

static Thread* next_ready(Thread* from) {
    if (!thread_list_head) return NULL;
    Thread* t = from ? from->next : thread_list_head;
    if (!t) t = thread_list_head;
    Thread* start = t;
    do {
        if (t->state != THREAD_FINISHED && t->state != THREAD_BLOCKED) {
            return t;
        }
        t = t->next ? t->next : thread_list_head;
    } while (t != start);
    return NULL;
}

void scheduler_switch(void) {
    if (!thread_list_head) return;

    if (!current_thread) {
        current_thread = next_ready(NULL);
        if (!current_thread) return;
        current_thread->state = THREAD_RUNNING;
        switch_context(NULL, current_thread);
        return;
    }

    Thread* old = current_thread;
    Thread* next = next_ready(old);
    if (!next || next == old) return;

    if (next->process && next->process->pml4) {
        vmm_switch(next->process->pml4);
    } else {
        vmm_switch(vmm_kernel_pml4());
    }

    if (old->state == THREAD_RUNNING) old->state = THREAD_READY;
    next->state = THREAD_RUNNING;
    current_thread = next;
    switch_context(old, next);
}

void scheduler_dump(void) {
    kprintln("Scheduler threads:");
    for (Thread* t = thread_list_head; t; t = t->next) {
        kprintln("  id=%d state=%d rsp=%p", (int)t->id, (int)t->state, (void*)t->rsp);
    }
}

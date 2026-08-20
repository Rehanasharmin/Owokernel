#include "thread.h"
#include "heap.h"
#include "pmm.h"
#include "scheduler.h"
#include "kernel.h"
#include "string.h"

static uint32_t next_thread_id = 0;

void thread_init(void) {}

Thread* thread_create(void (*entry)(void)) {
    Thread* t = (Thread*)kmalloc(sizeof(Thread));
    if (!t) return NULL;
    kmemset(t, 0, sizeof(Thread));

    uint8_t* stack = (uint8_t*)kmalloc(PAGE_SIZE);
    if (!stack) return NULL;

    t->id = next_thread_id++;
    t->state = THREAD_READY;
    t->stack_base = (uint64_t)stack;
    t->process = NULL;
    t->next = NULL;

    uint64_t* sp = (uint64_t*)(stack + PAGE_SIZE);
    *(--sp) = (uint64_t)entry;
    *(--sp) = 0; /* RBP */
    *(--sp) = 0; /* RBX */
    *(--sp) = 0; /* R12 */
    *(--sp) = 0; /* R13 */
    *(--sp) = 0; /* R14 */
    *(--sp) = 0; /* R15 */
    t->rsp = (uint64_t)sp;

    scheduler_add_thread(t);
    kprintln("Thread: created id=%d entry=%p", (int)t->id, (void*)entry);
    return t;
}

void thread_yield(void) {
    scheduler_switch();
}

#include "thread.h"
#include "pmm.h"
#include "vmm.h"
#include "scheduler.h"
#include "kernel.h"

static uint32_t next_thread_id = 0;

void thread_init() {
    // Scheduler is initialized in scheduler_init
}

Thread* thread_create(void (*entry)()) {
    // Allocate memory for the TCB
    Thread* t = (Thread*)pmm_alloc_page(); // Simple: one page per TCB for now
    if (!t) return NULL;

    // Allocate memory for the stack (one page)
    uint64_t stack_phys = (uint64_t)pmm_alloc_page();
    uint64_t stack_virt = 0xFFFF900000000000 + (next_thread_id * PAGE_SIZE * 2); // Rough mapping
    vmm_map(stack_virt, stack_phys, PAGE_WRITABLE);

    t->id = next_thread_id++;
    t->state = THREAD_READY;
    t->stack_base = stack_virt;
    t->next = NULL;

    /* 
     * Setup the initial stack.
     * We must "fake" a stack frame so that switch_context can 'pop' 
     * the registers and 'ret' into the entry function.
     */
    uint64_t* stack = (uint64_t*)(stack_virt + PAGE_SIZE);
    
    // Push the entry point
    *(--stack) = (uint64_t)entry;
    
    // Push fake registers for switch_context:
    // RBP, RBX, R12, R13, R14, R15
    *(--stack) = 0; // RBP
    *(--stack) = 0; // RBX
    *(--stack) = 0; // R12
    *(--stack) = 0; // R13
    *(--stack) = 0; // R14
    *(--stack) = 0; // R15

    t->rsp = (uint64_t)stack;

    scheduler_add_thread(t);
    return t;
}

void thread_yield() {
    scheduler_switch();
}

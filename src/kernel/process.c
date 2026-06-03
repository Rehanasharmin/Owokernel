#include "process.h"
#include "pmm.h"
#include "vmm.h"
#include "scheduler.h"
#include "kernel.h"

static uint32_t next_pid = 1;

Process* process_create(void (*entry)()) {
    Process* p = (Process*)pmm_alloc_page();
    if (!p) return NULL;

    p->pid = next_pid++;
    p->pml4 = vmm_create_address_space();

    /* Map user code and stack into the new address space */
    uint64_t user_code_phys = (uint64_t)pmm_alloc_page();
    uint64_t user_code_virt = 0x400000; // Standard user entry
    vmm_map_in_pml4(p->pml4, user_code_virt, user_code_phys, PAGE_WRITABLE | PAGE_USER);

    uint64_t user_stack_phys = (uint64_t)pmm_alloc_page();
    uint64_t user_stack_virt = 0x7FFFFFFFF000;
    vmm_map_in_pml4(p->pml4, user_stack_virt, user_stack_phys, PAGE_WRITABLE | PAGE_USER);

    /* Now create the thread associated with this process */
    // We'll use a modified thread_create that takes a PML4
    // For this demo, we'll manually setup the thread
    Thread* t = &p->thread;
    t->id = p->pid;
    t->state = THREAD_READY;
    t->stack_base = user_stack_virt;

    uint64_t* stack = (uint64_t*)(user_stack_virt + PAGE_SIZE);
    *(--stack) = (uint64_t)entry; // RIP
    *(--stack) = 0; // RBP
    *(--stack) = 0; // RBX
    *(--stack) = 0; // R12
    *(--stack) = 0; // R13
    *(--stack) = 0; // R14
    *(--stack) = 0; // R15
    t->rsp = (uint64_t)stack;

    scheduler_add_thread(t);
    return p;
}

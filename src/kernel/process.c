#include "process.h"
#include "pmm.h"
#include "vmm.h"
#include "scheduler.h"
#include "kernel.h"

static uint32_t next_pid = 1;

#include "process.h"
#include "pmm.h"
#include "vmm.h"
#include "scheduler.h"
#include "kernel.h"
#include "elf.h"

extern int elf_load(Process* proc, uint8_t* elf_data);

Process* process_create_elf(uint8_t* elf_data) {
    Process* p = (Process*)pmm_alloc_page();
    if (!p) return NULL;

    p->pid = 1; // Simple PID for now
    p->pml4 = vmm_create_address_space();

    /* Load the ELF binary into the process */
    if (elf_load(p, elf_data) != 0) {
        kprintln("Process: ELF load failed!");
        return NULL;
    }

    /* Get entry point from ELF header */
    Elf64_Ehdr* header = (Elf64_Ehdr*)elf_data;
    uint64_t entry_point = header->e_entry;

    /* Setup the thread's stack (in User Space) */
    uint64_t user_stack_phys = (uint64_t)pmm_alloc_page();
    uint64_t user_stack_virt = 0x7FFFFFFFF000;
    vmm_map_in_pml4(p->pml4, user_stack_virt, user_stack_phys, PAGE_WRITABLE | PAGE_USER);

    uint64_t* stack = (uint64_t*)(user_stack_virt + PAGE_SIZE);
    *(--stack) = (uint64_t)entry_point; // RIP
    *(--stack) = 0; // RBP
    *(--stack) = 0; // RBX
    *(--stack) = 0; // R12
    *(--stack) = 0; // R13
    *(--stack) = 0; // R14
    *(--stack) = 0; // R15
    
    p->thread.rsp = (uint64_t)stack;
    p->thread.id = p->pid;
    p->thread.state = THREAD_READY;
    p->thread.next = NULL;

    scheduler_add_thread(&p->thread);
    return p;
}

#include "process.h"
#include "pmm.h"
#include "vmm.h"
#include "scheduler.h"
#include "kernel.h"
#include "elf.h"
#include "heap.h"
#include "string.h"

static uint32_t next_pid = 1;

Process* process_create_elf(uint8_t* elf_data) {
    Process* p = (Process*)kmalloc(sizeof(Process));
    if (!p) return NULL;
    kmemset(p, 0, sizeof(Process));

    p->pid = next_pid++;
    p->pml4 = vmm_create_address_space();
    if (!p->pml4) return NULL;

    if (elf_load(p, elf_data) != 0) {
        kprintln("Process: ELF load failed!");
        return NULL;
    }

    Elf64_Ehdr* header = (Elf64_Ehdr*)elf_data;
    uint64_t entry_point = header->e_entry;

    uint64_t user_stack_phys = (uint64_t)pmm_alloc_page();
    uint64_t user_stack_virt = 0x7FFFFFFFE000ull;
    vmm_map_in_pml4(p->pml4, user_stack_virt, user_stack_phys, PAGE_WRITABLE | PAGE_USER);

    /* Kernel-visible mapping of the same physical stack so we can seed it. */
    uint64_t* stack = (uint64_t*)(user_stack_phys + PAGE_SIZE);
    *(--stack) = entry_point;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;

    p->thread.rsp = user_stack_virt + PAGE_SIZE - (7 * 8);
    p->thread.stack_base = user_stack_virt;
    p->thread.id = p->pid;
    p->thread.state = THREAD_READY;
    p->thread.process = p;
    p->thread.next = NULL;

    scheduler_add_thread(&p->thread);
    kprintln("Process: created pid=%d entry=%p", (int)p->pid, (void*)entry_point);
    return p;
}

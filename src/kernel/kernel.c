#include "kernel.h"
#include "gdt.h"
#include "idt.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"
#include "pic.h"
#include "interrupts.h"
#include "thread.h"
#include "scheduler.h"
#include "process.h"
#include "syscall.h"

// External setup function from syscall_entry.c
extern void setup_syscalls();

void user_app() {
    /* 
     * This function is running in User Mode (simulated via Ring 3).
     * It cannot use kprintln() because that's a kernel function.
     * It must use the syscall wrapper.
     */
    sys_print("Hello from User Land!\n");
    sys_print("I am using system calls to talk to the kernel.\n");
    sys_print("Now, I will exit gracefully.\n");
    sys_exit();
}

void kernel_main(void) {
    gdt_install();
    idt_install();
    pmm_init(128 * 1024 * 1024);
    pmm_mark_used(0, 16 * 1024 * 1024);
    vmm_init();
    for (uint64_t addr = 0; addr < 16 * 1024 * 1024; addr += PAGE_SIZE) {
        vmm_map(addr, addr, PAGE_WRITABLE);
    }
    heap_init();
    
    setup_syscalls();
    
    scheduler_init();
    kprintln("Creating user process...");
    process_create(user_app);
    
    pic_init();
    idt_set_gate(32, (uint64_t)isr32, 0x8E);
    idt_set_gate(33, (uint64_t)isr33, 0x8E);
    
    kprintln("Starting scheduler...");
    __asm__ volatile("sti");

    scheduler_switch();

    while (1) {
        __asm__ volatile("hlt");
    }
}

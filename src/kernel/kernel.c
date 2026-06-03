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

void user_task() {
    kprintln("I am running in a private address space!");
    
    /* Test Isolation: try to write to a kernel address */
    kprintln("Attempting to access kernel memory (should trigger panic)...");
    uint64_t* kernel_ptr = (uint64_t*)0x1000; // Kernel area
    *kernel_ptr = 0xDEADBEEF; 
    
    kprintln("Error: I accessed kernel memory!");
    while(1);
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
    
    scheduler_init();
    
    kprintln("Creating isolated process...");
    process_create(user_task);
    
    pic_init();
    idt_set_gate(32, (uint64_t)isr32, 0x8E);
    idt_set_gate(33, (uint64_t)isr33, 0x8E);
    
    kprintln("Starting isolated scheduler...");
    __asm__ volatile("sti");

    scheduler_switch();

    while (1) {
        __asm__ volatile("hlt");
    }
}

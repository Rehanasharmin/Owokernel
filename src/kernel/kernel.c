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
#include "vfs.h"
#include "net/net_stack.h"

extern void setup_syscalls();
extern void shell_main();

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
    vfs_init();
    net_init();
    
    scheduler_init();
    kprintln("Booting Owo Shell...");
    thread_create(shell_main);
    
    pic_init();
    idt_set_gate(32, (uint64_t)isr32, 0x8E);
    idt_set_gate(33, (uint64_t)isr33, 0x8E);
    
    kprintln("Kernel Ready. Welcome to Owokernel.");
    __asm__ volatile("sti");

    scheduler_switch();

    while (1) {
        __asm__ volatile("hlt");
    }
}

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
#include "drivers/driver.h"
#include "drivers/keyboard.h"
#include "drivers/timer.h"
#include "drivers/disk.h"

extern void setup_syscalls();

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
    
    /* Initialize Driver Framework */
    keyboard_init();
    timer_init();
    disk_init();
    kprintln("Driver Framework: All drivers initialized.");

    scheduler_init();
    kprintln("Starting scheduler...");
    __asm__ volatile("sti");

    scheduler_switch();

    while (1) {
        __asm__ volatile("hlt");
    }
}

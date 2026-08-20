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
#include "syscall.h"
#include "vfs.h"
#include "net/net_stack.h"
#include "drivers/keyboard.h"
#include "drivers/timer.h"
#include "drivers/disk.h"

extern void shell_main(void);

void kernel_main(void) {
    terminal_init();
    kprintln("Owokernel boot");

    gdt_install();
    kprintln("GDT installed");

    idt_install();
    kprintln("IDT installed");

    pmm_init(128ull * 1024 * 1024);
    /* Reserve low memory: BIOS, bitmap at 1MB, kernel at 16MB. */
    pmm_mark_used(0, 32ull * 1024 * 1024);

    vmm_init();
    for (uint64_t addr = 0; addr < 32ull * 1024 * 1024; addr += PAGE_SIZE) {
        vmm_map(addr, addr, PAGE_WRITABLE);
    }
    kprintln("Paging identity-mapped 32MB");

    heap_init();
    setup_syscalls();
    vfs_init();
    net_init();

    keyboard_init();
    timer_init();
    disk_init();

    scheduler_init();
    thread_create(shell_main);

    pic_init();

    kprintln("Kernel Ready. Welcome to Owokernel.");
    __asm__ volatile("sti");

    scheduler_switch();

    while (1) {
        __asm__ volatile("hlt");
    }
}

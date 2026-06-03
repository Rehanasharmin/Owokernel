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

extern void setup_syscalls();

void user_app() {
    sys_print("Testing VFS...\n");
    
    // 1. Open (create) a file
    int fd = sys_open("hello.txt");
    sys_print("Opened hello.txt with FD: ");
    // We don't have a sys_print_int, so just assume it works
    
    // 2. Write to the file
    sys_write(fd, "Hello from the RAMFS!", 20);
    sys_print("Wrote to file.\n");
    
    // 3. Close and Re-open to test persistence
    sys_close(fd);
    fd = sys_open("hello.txt");
    
    // 4. Read back from the file
    char buffer[64];
    // In a real user app, we'd need a way to map this buffer.
    // For this simulation, we are using a simple buffer.
    sys_read(fd, buffer, 64);
    sys_print("Read from file: ");
    sys_print(buffer);
    sys_print("\n");
    
    sys_print("VFS Test Complete. Exiting...\n");
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
    vfs_init();
    
    scheduler_init();
    kprintln("Creating VFS test process...");
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

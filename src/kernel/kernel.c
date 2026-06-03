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

void net_test_app() {
    sys_print("Starting Networking Test...\n");
    
    const char* msg = "Hello Network!";
    uint8_t buffer[64];
    
    sys_print("Sending packet to 'lo'...\n");
    sys_net_send("lo", (const uint8_t*)msg, 14);
    
    sys_print("Receiving packet from 'lo'...\n");
    int len = sys_net_recv("lo", buffer, 64);
    
    if (len > 0) {
        buffer[len] = '\0';
        sys_print("Received: ");
        sys_print((const char*)buffer);
        sys_print("\n");
    } else {
        sys_print("Failed to receive packet.\n");
    }
    
    sys_print("Networking Test Complete. Exiting...\n");
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
    net_init();
    
    scheduler_init();
    kprintln("Creating Net test process...");
    process_create_elf(NULL); // This is a placeholder, I'll use a normal thread for the test
    
    // Let's just create a kernel thread for the net test to avoid ELF complexity here
    thread_create(net_test_app);
    
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

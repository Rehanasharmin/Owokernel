#include "idt.h"
#include "kernel.h"
#include "interrupts.h"

struct idt_entry idt[256];
struct idt_ptr ip;

void idt_set_gate(uint8_t num, uint64_t isr, uint8_t flags) {
    idt[num].isr_low = isr & 0xFFFF;
    idt[num].isr_mid = (isr >> 16) & 0xFFFF;
    idt[num].isr_high = (isr >> 32) & 0xFFFF;
    idt[num].kernel_cs = 0x08; // GDT Code segment
    idt[num].attributes = flags;
    idt[num].ist = 0;
    idt[num].reserved = 0;
}

void idt_install() {
    ip.limit = (sizeof(struct idt_entry) * 256) - 1;
    ip.base = (uint64_t)&idt;

    /* Load the IDT */
    __asm__ volatile("lidt %0" : : "m"(ip));
}

/* This is the C handler that the assembly wrapper will call */
void common_interrupt_handler(uint64_t vector, uint64_t error_code) {
    if (vector >= 32) {
        irq_handler(vector, error_code);
        return;
    }

    kprintln("[PANIC] CPU Exception occurred!");
    kprintln("Vector: %d, Error Code: %d", vector, error_code);
    
    while(1) {
        __asm__ volatile("hlt");
    }
}

/* 
   Since we are using GCC, we can define the ISR wrappers 
   using global assembly. This saves us from needing a separate .asm file.
*/
/* 
   Updated ISR wrappers. 
   isr0: Exception 0
   isr32: IRQ 0 (Timer)
   isr33: IRQ 1 (Keyboard)
*/
__asm__(
    ".section .text\n"
    ".global isr0\n"
    "isr0:\n"
    "    pushq %rax; pushq %rcx; pushq %rdx; pushq %rsi; pushq %rdi; pushq %r8; pushq %r9; pushq %r10; pushq %r11; pushq %r12; pushq %r13; pushq %r14; pushq %r15\n"
    "    pushq $0\n"
    "    pushq $0\n"
    "    call common_interrupt_handler\n"
    "    popq %r15; popq %r14; popq %r13; popq %r12; popq %r11; popq %r10; popq %r9; popq %r8; popq %rdi; popq %rsi; popq %rdx; popq %rcx; popq %rax\n"
    "    addq $16, %rsp\n"
    "    iretq\n"
    
    ".global isr32\n"
    "isr32:\n"
    "    pushq %rax; pushq %rcx; pushq %rdx; pushq %rsi; pushq %rdi; pushq %r8; pushq %r9; pushq %r10; pushq %r11; pushq %r12; pushq %r13; pushq %r14; pushq %r15\n"
    "    pushq $0\n"
    "    pushq $32\n"
    "    call common_interrupt_handler\n"
    "    popq %r15; popq %r14; popq %r13; popq %r12; popq %r11; popq %r10; popq %r9; popq %r8; popq %rdi; popq %rsi; popq %rdx; popq %rcx; popq %rax\n"
    "    addq $16, %rsp\n"
    "    iretq\n"
    
    ".global isr33\n"
    "isr33:\n"
    "    pushq %rax; pushq %rcx; pushq %rdx; pushq %rsi; pushq %rdi; pushq %r8; pushq %r9; pushq %r10; pushq %r11; pushq %r12; pushq %r13; pushq %r14; pushq %r15\n"
    "    pushq $0\n"
    "    pushq $33\n"
    "    call common_interrupt_handler\n"
    "    popq %r15; popq %r14; popq %r13; popq %r12; popq %r11; popq %r10; popq %r9; popq %r8; popq %rdi; popq %rsi; popq %rdx; popq %rcx; popq %rax\n"
    "    addq $16, %rsp\n"
    "    iretq\n"
);

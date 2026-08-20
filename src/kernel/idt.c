#include "idt.h"
#include "kernel.h"
#include "interrupts.h"

static struct idt_entry idt[256];
static struct idt_ptr ip;

void idt_set_gate(uint8_t num, uint64_t isr, uint8_t flags) {
    idt[num].isr_low = isr & 0xFFFF;
    idt[num].isr_mid = (isr >> 16) & 0xFFFF;
    idt[num].isr_high = (isr >> 32) & 0xFFFFFFFF;
    idt[num].kernel_cs = 0x08;
    idt[num].attributes = flags;
    idt[num].ist = 0;
    idt[num].reserved = 0;
}

void common_interrupt_handler(uint64_t vector, uint64_t error_code) {
    if (vector >= 32) {
        irq_handler(vector, error_code);
        return;
    }

    static const char* names[] = {
        "Divide by zero", "Debug", "NMI", "Breakpoint", "Overflow",
        "Bound range", "Invalid opcode", "Device not available",
        "Double fault", "Coprocessor overrun", "Invalid TSS",
        "Segment not present", "Stack fault", "GPF", "Page fault",
        "Reserved", "x87 FP", "Alignment check", "Machine check",
        "SIMD FP"
    };
    const char* name = (vector < 20) ? names[vector] : "Unknown";
    kprintln("[PANIC] CPU exception: %s (vector %d, err %d)", name, (int)vector, (int)error_code);
    if (vector == 14) {
        uint64_t cr2;
        __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
        kprintln("CR2 (fault addr): %p", cr2);
    }
    while (1) {
        __asm__ volatile("hlt");
    }
}

#define ISR_NOERR(n, vec) \
    ".global isr" #n "\n" \
    "isr" #n ":\n" \
    "    pushq $0\n" \
    "    pushq $" #vec "\n" \
    "    jmp isr_common\n"

#define ISR_ERR(n, vec) \
    ".global isr" #n "\n" \
    "isr" #n ":\n" \
    "    pushq $" #vec "\n" \
    "    jmp isr_common\n"

__asm__(
    ".section .text\n"
    "isr_common:\n"
    "    pushq %rax\n"
    "    pushq %rbx\n"
    "    pushq %rcx\n"
    "    pushq %rdx\n"
    "    pushq %rsi\n"
    "    pushq %rdi\n"
    "    pushq %rbp\n"
    "    pushq %r8\n"
    "    pushq %r9\n"
    "    pushq %r10\n"
    "    pushq %r11\n"
    "    pushq %r12\n"
    "    pushq %r13\n"
    "    pushq %r14\n"
    "    pushq %r15\n"
    "    movq 120(%rsp), %rdi\n" /* vector */
    "    movq 128(%rsp), %rsi\n" /* error */
    "    call common_interrupt_handler\n"
    "    popq %r15\n"
    "    popq %r14\n"
    "    popq %r13\n"
    "    popq %r12\n"
    "    popq %r11\n"
    "    popq %r10\n"
    "    popq %r9\n"
    "    popq %r8\n"
    "    popq %rbp\n"
    "    popq %rdi\n"
    "    popq %rsi\n"
    "    popq %rdx\n"
    "    popq %rcx\n"
    "    popq %rbx\n"
    "    popq %rax\n"
    "    addq $16, %rsp\n"
    "    iretq\n"

    ISR_NOERR(0, 0)
    ISR_NOERR(1, 1)
    ISR_NOERR(2, 2)
    ISR_NOERR(3, 3)
    ISR_NOERR(4, 4)
    ISR_NOERR(5, 5)
    ISR_NOERR(6, 6)
    ISR_NOERR(7, 7)
    ISR_ERR(8, 8)
    ISR_NOERR(9, 9)
    ISR_ERR(10, 10)
    ISR_ERR(11, 11)
    ISR_ERR(12, 12)
    ISR_ERR(13, 13)
    ISR_ERR(14, 14)
    ISR_NOERR(15, 15)
    ISR_NOERR(16, 16)
    ISR_ERR(17, 17)
    ISR_NOERR(18, 18)
    ISR_NOERR(19, 19)
    ISR_NOERR(32, 32)
    ISR_NOERR(33, 33)
);

extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);

void idt_install(void) {
    ip.limit = (sizeof(struct idt_entry) * 256) - 1;
    ip.base = (uint64_t)&idt;

    uint64_t stubs[] = {
        (uint64_t)isr0, (uint64_t)isr1, (uint64_t)isr2, (uint64_t)isr3,
        (uint64_t)isr4, (uint64_t)isr5, (uint64_t)isr6, (uint64_t)isr7,
        (uint64_t)isr8, (uint64_t)isr9, (uint64_t)isr10, (uint64_t)isr11,
        (uint64_t)isr12, (uint64_t)isr13, (uint64_t)isr14, (uint64_t)isr15,
        (uint64_t)isr16, (uint64_t)isr17, (uint64_t)isr18, (uint64_t)isr19
    };
    for (int i = 0; i < 20; i++) {
        idt_set_gate((uint8_t)i, stubs[i], 0x8E);
    }
    idt_set_gate(32, (uint64_t)isr32, 0x8E);
    idt_set_gate(33, (uint64_t)isr33, 0x8E);

    __asm__ volatile("lidt %0" : : "m"(ip));
}

#include "syscall.h"
#include "gdt.h"
#include "kernel.h"

extern uint64_t syscall_handler(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3);

__asm__(
    ".global syscall_stub\n"
    "syscall_stub:\n"
    "    pushq %rcx\n"
    "    pushq %r11\n"
    "    pushq %rbx\n"
    "    pushq %rbp\n"
    "    pushq %r12\n"
    "    pushq %r13\n"
    "    pushq %r14\n"
    "    pushq %r15\n"
    "    movq %rdx, %rcx\n"
    "    movq %rsi, %rdx\n"
    "    movq %rdi, %rsi\n"
    "    movq %rax, %rdi\n"
    "    call syscall_handler\n"
    "    popq %r15\n"
    "    popq %r14\n"
    "    popq %r13\n"
    "    popq %r12\n"
    "    popq %rbp\n"
    "    popq %rbx\n"
    "    popq %r11\n"
    "    popq %rcx\n"
    "    sysretq\n"
);

static void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t lo = (uint32_t)value;
    uint32_t hi = (uint32_t)(value >> 32);
    __asm__ volatile("wrmsr" : : "c"(msr), "a"(lo), "d"(hi));
}

static uint64_t rdmsr(uint32_t msr) {
    uint32_t lo, hi;
    __asm__ volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
    return ((uint64_t)hi << 32) | lo;
}

extern void syscall_stub(void);

void setup_syscalls(void) {
    uint64_t efer = rdmsr(0xC0000080);
    efer |= 1;
    wrmsr(0xC0000080, efer);

    uint64_t star = ((uint64_t)GDT_KERNEL_CS << 32) | ((uint64_t)0x18 << 48);
    wrmsr(0xC0000081, star);
    wrmsr(0xC0000082, (uint64_t)syscall_stub);
    wrmsr(0xC0000084, 0x3F7);

    kprintln("Syscalls: EFER.SCE, STAR, LSTAR configured.");
}

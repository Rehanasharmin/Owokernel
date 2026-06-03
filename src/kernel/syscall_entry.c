#include "syscall.h"
#include "kernel.h"

extern void syscall_entry();

/* 
 * This is the entry point the CPU jumps to when 'syscall' is executed.
 * It must be written in assembly to save registers and handle the transition.
 */
__asm__(
    ".global syscall_entry\n"
    "syscall_entry:\n"
    "    # Save user registers\n"
    "    pushq %rcx\n" // syscall saves RIP in rcx
    "    pushq %r11\n" // syscall saves RFLAGS in r11
    "    pushq %rbx; pushq %rbp; pushq %r12; pushq %r13; pushq %r14; pushq %r15\n"
    "    \n"
    "    # Set up arguments for the C handler\n"
    "    # rax is already the syscall number\n"
    "    # rdi is the first argument\n"
    "    movq %rax, %rdi\n" 
    "    movq %rsi, %rsi\n" // second arg
    "    # We need to swap them to fit (syscall_num, arg1)\n"
    "    pushq %rsi\n"
    "    pushq %rax\n"
    "    call syscall_handler_wrapper\n"
    "    addq $16, %rsp\n"
    "    \n"
    "    # Restore registers\n"
    "    popq %r15; popq %r14; popq %r13; popq %r12; popq %rbp; popq %rbx\n"
    "    popq %r11\n"
    "    popq %rcx\n"
    "    \n"
    "    # Return to user mode\n"
    "    sysretq\n"
);

void syscall_handler_wrapper() {
    uint64_t num = 0;
    uint64_t arg = 0;
    __asm__ volatile("popq %0" : "=r"(num));
    __asm__ volatile("popq %0" : "=r"(arg));
    
    extern void syscall_handler(uint64_t syscall_num, uint64_t arg1);
    syscall_handler(num, arg);
}

void setup_syscalls() {
    uint64_t lstar = (uint64_t)syscall_entry;
    
    /* Write the address of syscall_entry to the LSTAR MSR (0xC0000082) */
    __asm__ volatile (
        "movq %0, %%rax\n"
        "movq %1, %%rdx\n"
        "wrmsr\n"
        : : "r"((uint64_t)0xC0000082), "r"((uint64_t)lstar) : "rax", "rdx"
    );
    kprintln("Syscalls: LSTAR MSR configured.");
}

#include "syscall.h"
#include "kernel.h"

extern void syscall_entry();
extern uint64_t syscall_handler(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3);

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
    "    # rsi is the second argument\n"
    "    # rdx is the third argument\n"
    "    \n"
    "    call syscall_handler_wrapper\n"
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
    uint64_t num, a1, a2, a3;
    
    __asm__ volatile("movq %%rax, %0" : "=r"(num));
    __asm__ volatile("movq %%rdi, %0" : "=r"(a1));
    __asm__ volatile("movq %%rsi, %0" : "=r"(a2));
    __asm__ volatile("movq %%rdx, %0" : "=r"(a3));
    
    uint64_t result = syscall_handler(num, a1, a2, a3);
    
    __asm__ volatile("movq %0, %%rax" : : "r"(result));
}

void setup_syscalls() {
    uint64_t lstar = (uint64_t)syscall_entry;
    
    __asm__ volatile (
        "movq %0, %%rax\n"
        "movq %1, %%rdx\n"
        "wrmsr\n"
        : : "r"((uint64_t)0xC0000082), "r"((uint64_t)lstar) : "rax", "rdx"
    );
    kprintln("Syscalls: LSTAR MSR configured.");
}

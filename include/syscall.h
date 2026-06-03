#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

/* Syscall Numbers */
#define SYS_PRINT    1
#define SYS_EXIT     2
#define SYS_KMALLOC  3

/* User-side wrappers (these would normally be in a user library) */
static inline void sys_print(const char* msg) {
    __asm__ volatile (
        "movq %1, %%rdi\n"
        "movq %0, %%rax\n"
        "syscall\n"
        : : "i"(SYS_PRINT), "r"(msg) : "rax", "rdi"
    );
}

static inline void sys_exit() {
    __asm__ volatile (
        "movq %0, %%rax\n"
        "syscall\n"
        : : "i"(SYS_EXIT) : "rax"
    );
}

#endif

#include "thread.h"

/*
 * switch_context(old, new)
 * rdi = old Thread* (may be NULL)
 * rsi = new Thread*
 */
__asm__(
    ".global switch_context\n"
    "switch_context:\n"
    "    test %rdi, %rdi\n"
    "    jz 1f\n"
    "    pushq %rbp\n"
    "    pushq %rbx\n"
    "    pushq %r12\n"
    "    pushq %r13\n"
    "    pushq %r14\n"
    "    pushq %r15\n"
    "    movq %rsp, 0(%rdi)\n"   /* old->rsp */
    "1:\n"
    "    movq 0(%rsi), %rsp\n"   /* new->rsp */
    "    popq %r15\n"
    "    popq %r14\n"
    "    popq %r13\n"
    "    popq %r12\n"
    "    popq %rbx\n"
    "    popq %rbp\n"
    "    ret\n"
);

void switch_context(Thread* old, Thread* new);

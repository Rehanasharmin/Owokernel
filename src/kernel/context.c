#include "thread.h"

/* 
 * switch_context(old_rsp, new_rsp)
 * This function saves the current registers to the old stack 
 * and loads registers from the new stack.
 */
__asm__(
    ".global switch_context\n"
    "switch_context:\n"
    "    # Save current context\n"
    "    pushq %rbp\n"
    "    pushq %rbx\n"
    "    pushq %r12\n"
    "    pushq %r13\n"
    "    pushq %r14\n"
    "    pushq %r15\n"
    "    \n"
    "    # Save current RSP to the first argument (rdi)\n"
    "    movq %rsp, (%rdi)\n"
    "    \n"
    "    # Load new RSP from the second argument (rsi)\n"
    "    movq %rsi, %rsp\n"
    "    \n"
    "    # Restore context from new stack\n"
    "    popq %r15\n"
    "    popq %r14\n"
    "    popq %r13\n"
    "    popq %r12\n"
    "    popq %rbx\n"
    "    popq %rbp\n"
    "    \n"
    "    ret\n"
);

void switch_context(Thread* old, Thread* new); 
// Note: The actual implementation is in assembly above. 
// The compiler just needs the prototype.

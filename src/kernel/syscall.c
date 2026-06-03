#include "syscall.h"
#include "kernel.h"
#include "heap.h"
#include "scheduler.h"
#include "thread.h"

/* The actual kernel function that handles printing */
void do_sys_print(const char* msg) {
    kprint("[User]: %s", msg);
}

/* The actual kernel function that handles exiting */
void do_sys_exit() {
    kprintln("Process requested exit.");
    while(1) {
        __asm__ volatile("hlt");
    }
}

/* The dispatcher: called by the syscall entry point */
void syscall_handler(uint64_t syscall_num, uint64_t arg1) {
    switch (syscall_num) {
        case SYS_PRINT:
            do_sys_print((const char*)arg1);
            break;
        case SYS_EXIT:
            do_sys_exit();
            break;
        default:
            kprintln("Unknown syscall: %d", syscall_num);
            break;
    }
}

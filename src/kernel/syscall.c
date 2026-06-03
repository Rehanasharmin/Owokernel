#include "syscall.h"
#include "kernel.h"
#include "heap.h"
#include "scheduler.h"
#include "thread.h"
#include "vfs.h"
#include "net/net_stack.h"

void do_sys_print(const char* msg) {
    kprint("[User]: %s", msg);
}

void do_sys_exit() {
    kprintln("Process requested exit.");
    while(1) {
        __asm__ volatile("hlt");
    }
}

uint64_t syscall_handler(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    switch (syscall_num) {
        case SYS_PRINT:
            do_sys_print((const char*)arg1);
            return 0;
        case SYS_EXIT:
            do_sys_exit();
            return 0;
        case SYS_OPEN:
            return (uint64_t)vfs_open((const char*)arg1);
        case SYS_WRITE:
            return (uint64_t)vfs_write((int)arg1, (const char*)arg2, (size_t)arg3);
        case SYS_READ:
            return (uint64_t)vfs_read((int)arg1, (char*)arg2, (size_t)arg3);
        case SYS_CLOSE:
            vfs_close((int)arg1);
            return 0;
        case SYS_NET_SEND:
            return (uint64_t)net_send_packet((const char*)arg1, (const uint8_t*)arg2, (size_t)arg3);
        case SYS_NET_RECV:
            return (uint64_t)net_recv_packet((const char*)arg1, (uint8_t*)arg2, (size_t)arg3);
        default:
            kprintln("Unknown syscall: %d", syscall_num);
            return -1;
    }
}

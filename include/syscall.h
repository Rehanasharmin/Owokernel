#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include <stddef.h>

/* Syscall Numbers */
#define SYS_PRINT    1
#define SYS_EXIT     2
#define SYS_OPEN     3
#define SYS_READ     4
#define SYS_WRITE    5
#define SYS_CLOSE    6

/* User-side wrappers */
void sys_print(const char* msg);
void sys_exit();
int sys_open(const char* path);
size_t sys_write(int fd, const char* buf, size_t size);
size_t sys_read(int fd, char* buf, size_t size);
void sys_close(int fd);

#endif

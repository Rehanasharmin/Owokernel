#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include <stddef.h>

#define SYS_PRINT    1
#define SYS_EXIT     2
#define SYS_OPEN     3
#define SYS_READ     4
#define SYS_WRITE    5
#define SYS_CLOSE    6
#define SYS_NET_SEND 7
#define SYS_NET_RECV 8
#define SYS_READ_KBD 9

void setup_syscalls(void);
uint64_t syscall_handler(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3);

void sys_print(const char* msg);
void sys_exit(void);
int sys_open(const char* path);
size_t sys_write(int fd, const char* buf, size_t size);
size_t sys_read(int fd, char* buf, size_t size);
void sys_close(int fd);
int sys_net_send(const char* dev, const uint8_t* data, size_t len);
int sys_net_recv(const char* dev, uint8_t* buf, size_t len);
char sys_read_kbd(void);

#endif

#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>

void terminal_init(void);
void terminal_clear(void);
void terminal_putchar(char c);
void kputc(char c);
void kprint(const char* fmt, ...);
void kprintln(const char* fmt, ...);

#endif

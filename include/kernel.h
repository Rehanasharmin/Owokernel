#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>

/* Basic logging */
void kprint(const char* fmt, ...);
void kprintln(const char* fmt, ...);

#endif

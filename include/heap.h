#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>

/* Initialize the kernel heap */
void heap_init();

/* Allocate memory from the kernel heap */
void* kmalloc(size_t size);

/* Free memory back to the kernel heap */
void kfree(void* ptr);

#endif

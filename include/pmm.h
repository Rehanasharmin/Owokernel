#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PAGE_SIZE 4096

/* Initialize the PMM with the total amount of memory available */
void pmm_init(uint64_t mem_size);

/* Allocate a single 4KB page of physical memory */
void* pmm_alloc_page();

/* Free a 4KB page of physical memory */
void pmm_free_page(void* ptr);

/* Mark a specific region of memory as used (e.g., the kernel itself) */
void pmm_mark_used(uint64_t addr, uint64_t size);

#endif

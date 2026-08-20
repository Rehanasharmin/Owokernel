#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096

void pmm_init(uint64_t mem_size);
void pmm_mark_used(uint64_t addr, uint64_t size);
void* pmm_alloc_page(void);
void pmm_free_page(void* ptr);
uint64_t pmm_total_pages(void);
uint64_t pmm_used_pages(void);

#endif

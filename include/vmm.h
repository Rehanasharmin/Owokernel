#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PAGE_PRESENT  (1ull << 0)
#define PAGE_WRITABLE (1ull << 1)
#define PAGE_USER     (1ull << 2)

#define PAGE_SIZE 4096

void vmm_init(void);
void vmm_map(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags);
void vmm_map_in_pml4(uint64_t* pml4, uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags);
void vmm_unmap(uint64_t virtual_addr);
uint64_t* vmm_create_address_space(void);
uint64_t* vmm_kernel_pml4(void);
void vmm_switch(uint64_t* pml4);

#endif

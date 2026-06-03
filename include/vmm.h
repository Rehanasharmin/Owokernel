#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Page Table Entry Flags */
#define PAGE_PRESENT (1 << 0)
#define PAGE_WRITABLE (1 << 1)
#define PAGE_USER (1 << 2)

/* Initialize the Virtual Memory Manager */
void vmm_init();

/* Map a virtual address to a physical address */
void vmm_map(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags);

/* Map a virtual address in a SPECIFIC PML4 */
void vmm_map_in_pml4(uint64_t* pml4, uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags);

/* Unmap a virtual address */
void vmm_unmap(uint64_t virtual_addr);

/* Create a new PML4 and copy kernel mappings into it */
uint64_t* vmm_create_address_space();

#endif

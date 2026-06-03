#include "vmm.h"
#include "pmm.h"
#include "kernel.h"

static uint64_t* kernel_pml4 = NULL;

static uint64_t* allocate_zeroed_page() {
    void* page = pmm_alloc_page();
    if (!page) return NULL;
    uint64_t* ptr = (uint64_t*)page;
    for (int i = 0; i < 512; i++) ptr[i] = 0;
    return ptr;
}

void vmm_map_in_pml4(uint64_t* pml4, uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags) {
    uint64_t pml4_idx = (virtual_addr >> 39) & 0x1FF;
    uint64_t pdpt_idx = (virtual_addr >> 30) & 0x1FF;
    uint64_t pd_idx   = (virtual_addr >> 21) & 0x1FF;
    uint64_t pt_idx   = (virtual_addr >> 12) & 0x1FF;

    if (!(pml4[pml4_idx] & PAGE_PRESENT)) {
        uint64_t* new_table = allocate_zeroed_page();
        pml4[pml4_idx] = (uint64_t)new_table | PAGE_PRESENT | PAGE_WRITABLE;
    }
    uint64_t* pdpt = (uint64_t*)(pml4[pml4_idx] & ~0xFFF);

    if (!(pdpt[pdpt_idx] & PAGE_PRESENT)) {
        uint64_t* new_table = allocate_zeroed_page();
        pdpt[pdpt_idx] = (uint64_t)new_table | PAGE_PRESENT | PAGE_WRITABLE;
    }
    uint64_t* pd = (uint64_t*)(pdpt[pdpt_idx] & ~0xFFF);

    if (!(pd[pd_idx] & PAGE_PRESENT)) {
        uint64_t* new_table = allocate_zeroed_page();
        pd[pd_idx] = (uint64_t)new_table | PAGE_PRESENT | PAGE_WRITABLE;
    }
    uint64_t* pt = (uint64_t*)(pd[pd_idx] & ~0xFFF);

    pt[pt_idx] = physical_addr | flags | PAGE_PRESENT;
}

void vmm_init() {
    kernel_pml4 = allocate_zeroed_page();
    __asm__ volatile("mov %0, %%cr3" : : "r"(kernel_pml4));
}

void vmm_map(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags) {
    vmm_map_in_pml4(kernel_pml4, virtual_addr, physical_addr, flags);
}

uint64_t* vmm_create_address_space() {
    uint64_t* new_pml4 = allocate_zeroed_page();
    /* Copy the upper half (kernel) from the main kernel_pml4 */
    for (int i = 256; i < 512; i++) {
        new_pml4[i] = kernel_pml4[i];
    }
    return new_pml4;
}

void vmm_unmap(uint64_t virtual_addr) {
    uint64_t pml4_idx = (virtual_addr >> 39) & 0x1FF;
    uint64_t pdpt_idx = (virtual_addr >> 30) & 0x1FF;
    uint64_t pd_idx   = (virtual_addr >> 21) & 0x1FF;
    uint64_t pt_idx   = (virtual_addr >> 12) & 0x1FF;

    uint64_t* pdpt = (uint64_t*)(kernel_pml4[pml4_idx] & ~0xFFF);
    uint64_t* pd = (uint64_t*)(pdpt[pdpt_idx] & ~0xFFF);
    uint64_t* pt = (uint64_t*)(pd[pd_idx] & ~0xFFF);

    pt[pt_idx] = 0;
}

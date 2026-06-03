#include "elf.h"
#include "process.h"
#include "vmm.h"
#include "pmm.h"
#include "kernel.h"

int elf_load(Process* proc, uint8_t* elf_data) {
    Elf64_Ehdr* header = (Elf64_Ehdr*)elf_data;

    /* 1. Verify ELF Magic Number */
    if (header->e_ident[0] != 0x7f || header->e_ident[1] != 'E' || 
        header->e_ident[2] != 'L' || header->e_ident[3] != 'F') {
        kprintln("ELF: Invalid magic number!");
        return -1;
    }

    /* 2. Verify 64-bit */
    if (header->e_ident[4] != 2) {
        kprintln("ELF: Only 64-bit binaries supported!");
        return -1;
    }

    kprintln("ELF: Loading binary, entry point: %p", (void*)header->e_entry);

    /* 3. Parse Program Headers */
    Elf64_Phdr* phdr = (Elf64_Phdr*)(elf_data + header->e_phoff);
    
    // We'll assume there are a few segments (usually 2-3)
    for (int i = 0; i < 5; i++) {
        if (phdr[i].p_type != PT_LOAD) continue;

        kprintln("ELF: Mapping segment %d: Virt %p -> Phys %p, Size %d", 
                 i, (void*)phdr[i].p_vaddr, (void*)phdr[i].p_paddr, phdr[i].p_filesz);

        /* Map the segment into the process's private PML4 */
        for (uint64_t offset = 0; offset < phdr[i].p_memsz; offset += PAGE_SIZE) {
            uint64_t vaddr = phdr[i].p_vaddr + offset;
            uint64_t paddr = (uint64_t)pmm_alloc_page();
            
            vmm_map_in_pml4(proc->pml4, vaddr, paddr, PAGE_WRITABLE | PAGE_USER);
            
            // Copy data from ELF buffer to the physical page
            if (offset < phdr[i].p_filesz) {
                uint8_t* dest = (uint8_t*)paddr;
                uint8_t* src = elf_data + phdr[i].p_offset + offset;
                size_t size = (phdr[i].p_filesz - offset < PAGE_SIZE) ? 
                              (phdr[i].p_filesz - offset) : PAGE_SIZE;
                
                for (size_t j = 0; j < size; j++) dest[j] = src[j];
            }
        }
    }

    /* 4. Set the entry point for the process's main thread */
    proc->thread.rsp = 0; // We will set this up in process_create
    // We'll store the entry point in a temporary place or pass it back
    return 0; 
}

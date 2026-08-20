#include "elf.h"
#include "process.h"
#include "vmm.h"
#include "pmm.h"
#include "kernel.h"
#include "string.h"

int elf_load(Process* proc, uint8_t* elf_data) {
    Elf64_Ehdr* header = (Elf64_Ehdr*)elf_data;

    if (header->e_ident[0] != ELFMAG0 || header->e_ident[1] != ELFMAG1 ||
        header->e_ident[2] != ELFMAG2 || header->e_ident[3] != ELFMAG3) {
        kprintln("ELF: Invalid magic number!");
        return -1;
    }
    if (header->e_ident[4] != ELFCLASS64) {
        kprintln("ELF: Only 64-bit binaries supported!");
        return -1;
    }
    if (header->e_machine != EM_X86_64) {
        kprintln("ELF: Only x86_64 binaries supported!");
        return -1;
    }

    kprintln("ELF: Loading binary, entry point: %p", (void*)header->e_entry);

    Elf64_Phdr* phdr = (Elf64_Phdr*)(elf_data + header->e_phoff);
    for (Elf64_Half i = 0; i < header->e_phnum; i++) {
        if (phdr[i].p_type != PT_LOAD) continue;

        kprintln("ELF: Mapping segment %d virt=%p filesz=%d memsz=%d",
                 (int)i, (void*)phdr[i].p_vaddr, (int)phdr[i].p_filesz, (int)phdr[i].p_memsz);

        uint64_t flags = PAGE_USER | PAGE_WRITABLE;
        uint64_t vaddr_start = phdr[i].p_vaddr & ~(uint64_t)(PAGE_SIZE - 1);
        uint64_t vaddr_end = (phdr[i].p_vaddr + phdr[i].p_memsz + PAGE_SIZE - 1) & ~(uint64_t)(PAGE_SIZE - 1);

        for (uint64_t va = vaddr_start; va < vaddr_end; va += PAGE_SIZE) {
            uint64_t paddr = (uint64_t)pmm_alloc_page();
            kmemset((void*)paddr, 0, PAGE_SIZE);
            vmm_map_in_pml4(proc->pml4, va, paddr, flags);

            uint64_t file_off = va - phdr[i].p_vaddr;
            if (va + PAGE_SIZE > phdr[i].p_vaddr && file_off < phdr[i].p_filesz) {
                uint64_t copy_start = (va < phdr[i].p_vaddr) ? (phdr[i].p_vaddr - va) : 0;
                uint64_t src_off = phdr[i].p_offset + ((va + copy_start) - phdr[i].p_vaddr);
                uint64_t copy_len = PAGE_SIZE - copy_start;
                uint64_t remaining = phdr[i].p_filesz - ((va + copy_start) - phdr[i].p_vaddr);
                if (copy_len > remaining) copy_len = remaining;
                kmemcpy((uint8_t*)paddr + copy_start, elf_data + src_off, copy_len);
            }
        }
    }
    return 0;
}

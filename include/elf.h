#ifndef ELF_H
#define ELF_H

#include <stdint.h>

typedef uint8_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Xword;

typedef struct {
    unsigned char e_ident[16];
    Elf64_Half    e_type;
    Elf64_Half    e_machine;
    Elf64_Word    e_version;
    Elf64_Addr    e_entry;
    Elf64_Addr    e_phoff;
    Elf64_Addr    e_shoff;
    Elf64_Word    e_flags;
    Elf64_Half    e_ehsize;
    Elf64_Half    e_phesize;
    Elf64_Half    e_symoff;
    Elf64_Half    e_symsize;
    Elf64_Half    e_strtaboff;
    Elf64_Half    e_strtabsize;
    Elf64_Half    e_sectionheaderoff;
    Elf64_Half    e_sectionhdrsize;
    Elf64_Half    e_shentsize;
    Elf64_Half    e_shnum;
    Elf64_Half    e_shstrndx;
} Elf64_Ehdr;

typedef struct {
    Elf64_Word    p_type;
    Elf64_Word    p_flags;
    Elf64_Addr    p_offset;
    Elf64_Addr    p_vaddr;
    Elf64_Addr    p_paddr;
    Elf64_Xword   p_filesz;
    Elf64_Xword   p_memsz;
    Elf64_Xword   p_align;
} Elf64_Phdr;

#define PT_LOAD 1

#endif

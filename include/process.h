#ifndef PROCESS_H
#define PROCESS_H

#include "thread.h"
#include "vmm.h"

typedef struct Process {
    Thread thread;
    uint64_t* pml4;
    uint32_t pid;
} Process;

Process* process_create_elf(uint8_t* elf_data);

#endif

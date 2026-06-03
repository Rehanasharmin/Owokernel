#ifndef PROCESS_H
#define PROCESS_H

#include "thread.h"
#include "vmm.h"

typedef struct Process {
    Thread thread;           // The main thread of the process
    uint64_t* pml4;          // Private address space
    uint32_t pid;            // Process ID
} Process;

Process* process_create(void (*entry)());
void process_switch(Process* next);

#endif

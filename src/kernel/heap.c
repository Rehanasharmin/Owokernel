#include "heap.h"
#include "vmm.h"
#include "pmm.h"
#include "kernel.h"

typedef struct HeapBlock {
    size_t size;
    bool is_free;
    struct HeapBlock* next;
} HeapBlock;

static HeapBlock* heap_start = NULL;
static uint64_t heap_virt_base = 0xFFFF900000000000;

void heap_init() {
    /* 
     * Initial heap size: 1 MB 
     * We map 1MB of virtual memory to physical pages.
     */
    for (uint64_t addr = 0; addr < 1024 * 1024; addr += PAGE_SIZE) {
        void* phys = pmm_alloc_page();
        vmm_map(heap_virt_base + addr, (uint64_t)phys, PAGE_WRITABLE);
    }

    heap_start = (HeapBlock*)heap_virt_base;
    heap_start->size = (1024 * 1024) - sizeof(HeapBlock);
    heap_start->is_free = true;
    heap_start->next = NULL;

    kprintln("Heap: Initialized 1MB at %p", (void*)heap_virt_base);
}

void* kmalloc(size_t size) {
    /* Align size to 8 bytes for CPU performance */
    size = (size + 7) & ~7;

    HeapBlock* current = heap_start;
    while (current) {
        if (current->is_free && current->size >= size) {
            /* Split block if there is enough space for a new header + some data */
            if (current->size > size + sizeof(HeapBlock) + 8) {
                HeapBlock* next_block = (HeapBlock*)((uint8_t*)current + sizeof(HeapBlock) + size);
                next_block->size = current->size - size - sizeof(HeapBlock);
                next_block->is_free = true;
                next_block->next = current->next;

                current->size = size;
                current->next = next_block;
            }

            current->is_free = false;
            return (void*)((uint8_t*)current + sizeof(HeapBlock));
        }
        current = current->next;
    }

    kprintln("Heap: Out of memory!");
    return NULL;
}

void kfree(void* ptr) {
    if (!ptr) return;

    HeapBlock* block = (HeapBlock*)((uint8_t*)ptr - sizeof(HeapBlock));
    block->is_free = true;

    /* Coalesce adjacent free blocks */
    HeapBlock* current = heap_start;
    while (current && current->next) {
        if (current->is_free && current->next->is_free) {
            current->size += sizeof(HeapBlock) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

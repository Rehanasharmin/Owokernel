#include "pmm.h"
#include "kernel.h"

static uint8_t* bitmap = (uint8_t*)0x100000;
static uint64_t total_pages = 0;
static uint64_t used_pages = 0;

void pmm_init(uint64_t mem_size) {
    total_pages = mem_size / PAGE_SIZE;
    uint64_t bitmap_size = (total_pages + 7) / 8;
    used_pages = 0;

    kprintln("PMM: Initializing... Total RAM: %d MB", (int)(mem_size / (1024 * 1024)));
    kprintln("PMM: Managing %d pages.", (int)total_pages);

    for (uint64_t i = 0; i < bitmap_size; i++) {
        bitmap[i] = 0;
    }
}

void pmm_mark_used(uint64_t addr, uint64_t size) {
    uint64_t start_page = addr / PAGE_SIZE;
    uint64_t end_page = (addr + size + PAGE_SIZE - 1) / PAGE_SIZE;
    if (end_page > total_pages) end_page = total_pages;

    for (uint64_t i = start_page; i < end_page; i++) {
        if (!(bitmap[i / 8] & (1 << (i % 8)))) {
            bitmap[i / 8] |= (1 << (i % 8));
            used_pages++;
        }
    }
}

void* pmm_alloc_page(void) {
    uint64_t bitmap_size = (total_pages + 7) / 8;
    for (uint64_t i = 0; i < bitmap_size; i++) {
        if (bitmap[i] != 0xFF) {
            for (int bit = 0; bit < 8; bit++) {
                if (!(bitmap[i] & (1 << bit))) {
                    uint64_t page_idx = i * 8 + bit;
                    if (page_idx >= total_pages) return NULL;
                    bitmap[i] |= (1 << bit);
                    used_pages++;
                    return (void*)(page_idx * PAGE_SIZE);
                }
            }
        }
    }
    return NULL;
}

void pmm_free_page(void* ptr) {
    if (!ptr) return;
    uint64_t page_idx = (uint64_t)ptr / PAGE_SIZE;
    if (page_idx >= total_pages) return;
    if (bitmap[page_idx / 8] & (1 << (page_idx % 8))) {
        bitmap[page_idx / 8] &= (uint8_t)~(1 << (page_idx % 8));
        if (used_pages) used_pages--;
    }
}

uint64_t pmm_total_pages(void) { return total_pages; }
uint64_t pmm_used_pages(void) { return used_pages; }

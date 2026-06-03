#include "pmm.h"
#include "kernel.h"

/* 
 * The bitmap will be stored at a fixed location.
 * In a real kernel, we'd get this from the Limine memory map.
 * For now, we'll place it at 1MB to keep it away from the BIOS/Bootloader.
 */
static uint64_t* bitmap = (uint64_t*)0x100000; 
static uint64_t total_pages = 0;
static uint64_t bitmap_pages = 0;

void pmm_init(uint64_t mem_size) {
    total_pages = mem_size / PAGE_SIZE;
    
    /* 
     * Calculate how many pages the bitmap itself needs.
     * 1 bit per page. 64 bits per uint64_t.
     */
    bitmap_pages = (total_pages + 63) / 64 / (PAGE_SIZE / 8); // This is simplified
    // Let's use a simpler calculation for the bitmap size in bytes
    uint64_t bitmap_size_bytes = (total_pages + 7) / 8;
    
    kprintln("PMM: Initializing... Total RAM: %d MB", mem_size / (1024 * 1024));
    kprintln("PMM: Managing %d pages.", total_pages);

    /* Clear the bitmap (mark all as free) */
    uint8_t* bitmap_bytes = (uint8_t*)bitmap;
    for (uint64_t i = 0; i < bitmap_size_bytes; i++) {
        bitmap_bytes[i] = 0;
    }
}

void pmm_mark_used(uint64_t addr, uint64_t size) {
    uint64_t start_page = addr / PAGE_SIZE;
    uint64_t end_page = (addr + size + PAGE_SIZE - 1) / PAGE_SIZE;
    
    uint8_t* bitmap_bytes = (uint8_t*)bitmap;
    for (uint64_t i = start_page; i < end_page; i++) {
        bitmap_bytes[i / 8] |= (1 << (i % 8));
    }
}

void* pmm_alloc_page() {
    uint8_t* bitmap_bytes = (uint8_t*)bitmap;
    uint64_t bitmap_size_bytes = (total_pages + 7) / 8;

    for (uint64_t i = 0; i < bitmap_size_bytes; i++) {
        if (bitmap_bytes[i] != 0xFF) { // If at least one bit is 0
            for (int bit = 0; bit < 8; bit++) {
                if (!(bitmap_bytes[i] & (1 << bit))) {
                    uint64_t page_idx = i * 8 + bit;
                    if (page_idx >= total_pages) return NULL;
                    
                    bitmap_bytes[i] |= (1 << bit); // Mark as used
                    return (void*)(page_idx * PAGE_SIZE);
                }
            }
        }
    }
    return NULL; // Out of memory
}

void pmm_free_page(void* ptr) {
    uint64_t addr = (uint64_t)ptr;
    uint64_t page_idx = addr / PAGE_SIZE;
    
    uint8_t* bitmap_bytes = (uint8_t*)bitmap;
    bitmap_bytes[page_idx / 8] &= ~(1 << (page_idx % 8));
}

#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct idt_entry {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t reserved;
    uint8_t ist;
    uint16_t isr_mid;
    uint16_t attributes;
    uint16_t isr_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

extern void isr0();
extern void isr32();
extern void isr33();

void idt_install();
void idt_set_gate(uint8_t num, uint64_t isr, uint8_t flags);

#endif

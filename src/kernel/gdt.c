#include "gdt.h"

static struct gdt_entry gdt[5];
static struct gdt_ptr gp;

static void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_install(void) {
    gp.limit = (sizeof(struct gdt_entry) * 5) - 1;
    gp.base = (uint64_t)&gdt;

    gdt_set_gate(0, 0, 0, 0, 0);
    gdt_set_gate(1, 0, 0, 0x9A, 0xA0); /* kernel code, L=1 */
    gdt_set_gate(2, 0, 0, 0x92, 0xA0); /* kernel data */
    gdt_set_gate(3, 0, 0, 0xF2, 0xA0); /* user data */
    gdt_set_gate(4, 0, 0, 0xFA, 0xA0); /* user code, L=1 */

    __asm__ volatile(
        "lgdt %0\n"
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%ss\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "pushq $0x08\n"
        "leaq 1f(%%rip), %%rax\n"
        "pushq %%rax\n"
        "lretq\n"
        "1:\n"
        : : "m"(gp) : "rax", "memory"
    );
}

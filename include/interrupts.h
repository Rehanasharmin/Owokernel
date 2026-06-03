#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

void irq_handler(uint64_t vector, uint64_t error_code);
void keyboard_handler();
void timer_handler();

#endif

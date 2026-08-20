#include "interrupts.h"
#include "pic.h"
#include "io.h"
#include "kernel.h"
#include "scheduler.h"
#include "drivers/keyboard.h"

static uint64_t ticks = 0;

void timer_handler(void) {
    ticks++;
    pic_send_eoi(0);
    if ((ticks % 8) == 0) {
        scheduler_switch();
    }
}

void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);
    keyboard_handle_scancode(scancode);
    pic_send_eoi(1);
}

void irq_handler(uint64_t vector, uint64_t error_code) {
    (void)error_code;
    if (vector == 32) {
        timer_handler();
    } else if (vector == 33) {
        keyboard_handler();
    } else {
        kprintln("Unhandled IRQ: %d", (int)vector);
        pic_send_eoi((uint8_t)(vector - 32));
    }
}

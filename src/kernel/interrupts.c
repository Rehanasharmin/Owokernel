#include "interrupts.h"
#include "pic.h"
#include "io.h"
#include "kernel.h"
#include "scheduler.h"
#include "drivers/keyboard.h"

void timer_handler() {
    scheduler_switch();
    pic_send_eoi(0);
}

void keyboard_handler() {
    uint8_t scancode = inb(0x60);
    
    if (scancode == 0x1E) keyboard_put_char('A');
    else if (scancode == 0x30) keyboard_put_char('B');
    else if (scancode == 0x2E) keyboard_put_char('C');
    else if (scancode == 0x12) keyboard_put_char('W');
    else if (scancode == 0x24) keyboard_put_char('X');
    else if (scancode == 0x1C) kprintln(" [Enter]");
    else kprint(".");

    pic_send_eoi(1);
}

void irq_handler(uint64_t vector, uint64_t error_code) {
    if (vector == 32) {
        timer_handler();
    } else if (vector == 33) {
        keyboard_handler();
    } else {
        kprintln("Unhandled IRQ: %d", vector);
        pic_send_eoi(vector - 32);
    }
}

#include "drivers/timer.h"
#include "drivers/driver.h"
#include "io.h"
#include "kernel.h"

static int do_timer_init() {
    uint32_t divisor = 1193182 / 100;
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
    return 0;
}

Driver timer_driver = {
    .name = "timer",
    .init = do_timer_init,
    .read = NULL,
    .write = NULL
};

void timer_init() {
    driver_register(&timer_driver);
    do_timer_init();
}

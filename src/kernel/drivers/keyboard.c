#include "drivers/keyboard.h"
#include "drivers/driver.h"
#include "io.h"
#include "kernel.h"

static char keyboard_buffer[256];
static int buffer_head = 0;
static int buffer_tail = 0;

void keyboard_put_char(char c) {
    keyboard_buffer[buffer_head] = c;
    buffer_head = (buffer_head + 1) % 256;
}

char keyboard_get_char() {
    if (buffer_head == buffer_tail) return 0;
    char c = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % 256;
    return c;
}

static int kb_init() { return 0; }
static int kb_read(void* buf, size_t size) {
    char* b = (char*)buf;
    size_t read = 0;
    while (read < size && keyboard_get_char() != 0) {
        b[read++] = keyboard_get_char();
    }
    return read;
}

Driver keyboard_driver = {
    .name = "keyboard",
    .init = kb_init,
    .read = kb_read,
    .write = NULL
};

void keyboard_init() {
    driver_register(&keyboard_driver);
}

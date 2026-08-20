#include "drivers/keyboard.h"
#include "drivers/driver.h"
#include "io.h"
#include "kernel.h"

static char keyboard_buffer[256];
static int buffer_head = 0;
static int buffer_tail = 0;
static int shift = 0;

static const char scancode_map[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ',
};

static const char scancode_shift[128] = {
    0,  27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
    'A','S','D','F','G','H','J','K','L',':','"','~', 0, '|',
    'Z','X','C','V','B','N','M','<','>','?', 0, '*', 0, ' ',
};

void keyboard_put_char(char c) {
    int next = (buffer_head + 1) % 256;
    if (next == buffer_tail) return;
    keyboard_buffer[buffer_head] = c;
    buffer_head = next;
}

char keyboard_get_char(void) {
    if (buffer_head == buffer_tail) return 0;
    char c = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % 256;
    return c;
}

void keyboard_handle_scancode(uint8_t scancode) {
    if (scancode == 0x2A || scancode == 0x36) { shift = 1; return; }
    if (scancode == 0xAA || scancode == 0xB6) { shift = 0; return; }
    if (scancode & 0x80) return;
    char c = shift ? scancode_shift[scancode] : scancode_map[scancode];
    if (c) keyboard_put_char(c);
}

static int kb_init(void) { return 0; }

static int kb_read(void* buf, size_t size) {
    char* b = (char*)buf;
    size_t n = 0;
    while (n < size) {
        char c = keyboard_get_char();
        if (!c) break;
        b[n++] = c;
    }
    return (int)n;
}

Driver keyboard_driver = {
    .name = "keyboard",
    .init = kb_init,
    .read = kb_read,
    .write = NULL
};

void keyboard_init(void) {
    driver_register(&keyboard_driver);
}

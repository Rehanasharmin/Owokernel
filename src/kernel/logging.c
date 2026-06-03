#include "kernel.h"
#include <stdarg.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static volatile uint16_t* const VGA_BUFFER = (uint16_t*)0xB8000;

void terminal_putchar(char c, uint8_t color) {
    static size_t terminal_row = 0;
    static size_t terminal_col = 0;

    const size_t index = terminal_row * VGA_WIDTH + terminal_col;
    VGA_BUFFER[index] = (uint16_t)c | (uint16_t)color << 8;
    
    if (c == '\n') {
        terminal_col = 0;
        terminal_row++;
    } else {
        if (terminal_col == VGA_WIDTH - 1) {
            terminal_col = 0;
            terminal_row++;
        } else {
            terminal_col++;
        }
    }

    if (terminal_row == VGA_HEIGHT) {
        terminal_row = 0;
        terminal_col = 0;
    }
}

void kprint(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (const char* p = fmt; *p != '\0'; p++) {
        if (*p == '%') {
            p++;
            if (*p == 'd') {
                int d = va_arg(args, int);
                if (d == 0) terminal_putchar('0', 0x07);
                else {
                    char buf[12];
                    int i = 0;
                    while (d > 0) { buf[i++] = (d % 10) + '0'; d /= 10; }
                    while (i > 0) terminal_putchar(buf[--i], 0x07);
                }
            } else if (*p == 's') {
                const char* s = va_arg(args, const char*);
                while (*s) terminal_putchar(*s++, 0x07);
            } else if (*p == 'p') {
                uint64_t ptr = va_arg(args, uint64_t);
                terminal_putchar('0', 0x07);
                terminal_putchar('x', 0x07);
                const char* hex = "0123456789ABCDEF";
                for (int i = 15; i >= 0; i--) {
                    terminal_putchar(hex[(ptr >> (i * 4)) & 0xF], 0x07);
                }
            }
        } else {
            terminal_putchar(*p, 0x07);
        }
    }
    va_end(args);
}

void kprintln(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buf[512];
    int pos = 0;
    for (const char* p = fmt; *p != '\0' && pos < 511; p++) {
        if (*p == '%') {
            p++;
            if (*p == 'd') {
                int d = va_arg(args, int);
                if (d == 0) buf[pos++] = '0';
                else {
                    char tmp[12];
                    int i = 0;
                    while (d > 0 && pos < 511) { tmp[i++] = (d % 10) + '0'; d /= 10; }
                    while (i > 0 && pos < 511) buf[pos++] = tmp[--i];
                }
            } else if (*p == 's') {
                const char* s = va_arg(args, const char*);
                while (*s && pos < 511) buf[pos++] = *s++;
            } else if (*p == 'p') {
                uint64_t ptr = va_arg(args, uint64_t);
                buf[pos++] = '0';
                buf[pos++] = 'x';
                const char* hex = "0123456789ABCDEF";
                for (int i = 15; i >= 0; i--) {
                    if (pos < 511) buf[pos++] = hex[(ptr >> (i * 4)) & 0xF];
                }
            }
        } else {
            buf[pos++] = *p;
        }
    }
    buf[pos] = '\0';
    
    for(int i=0; i<pos; i++) terminal_putchar(buf[i], 0x07);
    terminal_putchar('\n', 0x07);
    
    va_end(args);
}

#include "kernel.h"
#include <stdarg.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static volatile uint16_t* const VGA_BUFFER = (uint16_t*)0xB8000;
static size_t terminal_row = 0;
static size_t terminal_col = 0;
static uint8_t terminal_color = 0x07;

static void terminal_scroll(void) {
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[(y - 1) * VGA_WIDTH + x] = VGA_BUFFER[y * VGA_WIDTH + x];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = (uint16_t)' ' | ((uint16_t)terminal_color << 8);
    }
    terminal_row = VGA_HEIGHT - 1;
}

void terminal_clear(void) {
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_BUFFER[i] = (uint16_t)' ' | ((uint16_t)terminal_color << 8);
    }
    terminal_row = 0;
    terminal_col = 0;
}

void terminal_init(void) {
    terminal_clear();
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_col = 0;
        terminal_row++;
    } else if (c == '\r') {
        terminal_col = 0;
    } else if (c == '\b') {
        if (terminal_col > 0) {
            terminal_col--;
            VGA_BUFFER[terminal_row * VGA_WIDTH + terminal_col] =
                (uint16_t)' ' | ((uint16_t)terminal_color << 8);
        }
    } else if (c == '\t') {
        terminal_col = (terminal_col + 4) & ~(size_t)3;
        if (terminal_col >= VGA_WIDTH) {
            terminal_col = 0;
            terminal_row++;
        }
    } else {
        VGA_BUFFER[terminal_row * VGA_WIDTH + terminal_col] =
            (uint16_t)c | ((uint16_t)terminal_color << 8);
        if (++terminal_col == VGA_WIDTH) {
            terminal_col = 0;
            terminal_row++;
        }
    }
    if (terminal_row >= VGA_HEIGHT) {
        terminal_scroll();
    }
}

void kputc(char c) {
    terminal_putchar(c);
}

static void print_uint(uint64_t v, int base, int width) {
    char tmp[32];
    const char* hex = "0123456789abcdef";
    int i = 0;
    if (v == 0) tmp[i++] = '0';
    while (v && i < 31) {
        tmp[i++] = hex[v % (uint64_t)base];
        v /= (uint64_t)base;
    }
    while (i < width) tmp[i++] = '0';
    while (i > 0) terminal_putchar(tmp[--i]);
}

static void vprint(const char* fmt, va_list args) {
    for (const char* p = fmt; p && *p; p++) {
        if (*p != '%') {
            terminal_putchar(*p);
            continue;
        }
        p++;
        if (*p == 'd' || *p == 'i') {
            int d = va_arg(args, int);
            if (d < 0) {
                terminal_putchar('-');
                print_uint((uint64_t)(-(int64_t)d), 10, 0);
            } else {
                print_uint((uint64_t)d, 10, 0);
            }
        } else if (*p == 'u') {
            print_uint(va_arg(args, unsigned int), 10, 0);
        } else if (*p == 'x') {
            print_uint(va_arg(args, unsigned int), 16, 0);
        } else if (*p == 's') {
            const char* s = va_arg(args, const char*);
            if (!s) s = "(null)";
            while (*s) terminal_putchar(*s++);
        } else if (*p == 'c') {
            terminal_putchar((char)va_arg(args, int));
        } else if (*p == 'p') {
            terminal_putchar('0');
            terminal_putchar('x');
            print_uint(va_arg(args, uint64_t), 16, 16);
        } else if (*p == '%') {
            terminal_putchar('%');
        } else {
            terminal_putchar('%');
            terminal_putchar(*p);
        }
    }
}

void kprint(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprint(fmt, args);
    va_end(args);
}

void kprintln(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprint(fmt, args);
    va_end(args);
    terminal_putchar('\n');
}

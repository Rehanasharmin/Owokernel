#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_init(void);
void keyboard_put_char(char c);
char keyboard_get_char(void);
void keyboard_handle_scancode(uint8_t scancode);

#endif

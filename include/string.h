#ifndef OWO_STRING_H
#define OWO_STRING_H

#include <stddef.h>
#include <stdint.h>

size_t kstrlen(const char* s);
int kstrcmp(const char* a, const char* b);
int kstrncmp(const char* a, const char* b, size_t n);
char* kstrcpy(char* dst, const char* src);
void* kmemset(void* dst, int c, size_t n);
void* kmemcpy(void* dst, const void* src, size_t n);

#endif

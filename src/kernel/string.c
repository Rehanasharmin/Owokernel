#include "string.h"

size_t kstrlen(const char* s) {
    size_t n = 0;
    while (s && s[n]) n++;
    return n;
}

int kstrcmp(const char* a, const char* b) {
    if (!a || !b) return (a == b) ? 0 : (a ? 1 : -1);
    while (*a && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

int kstrncmp(const char* a, const char* b, size_t n) {
    while (n && *a && *a == *b) { a++; b++; n--; }
    if (n == 0) return 0;
    return (unsigned char)*a - (unsigned char)*b;
}

char* kstrcpy(char* dst, const char* src) {
    char* d = dst;
    while ((*d++ = *src++));
    return dst;
}

void* kmemset(void* dst, int c, size_t n) {
    uint8_t* p = (uint8_t*)dst;
    for (size_t i = 0; i < n; i++) p[i] = (uint8_t)c;
    return dst;
}

void* kmemcpy(void* dst, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dst;
    const uint8_t* s = (const uint8_t*)src;
    for (size_t i = 0; i < n; i++) d[i] = s[i];
    return dst;
}

#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>
#include <stddef.h>

typedef struct Driver {
    const char* name;
    int (*init)();
    int (*read)(void* buf, size_t size);
    int (*write)(const void* buf, size_t size);
} Driver;

void driver_register(Driver* driver);
Driver* driver_get(const char* name);

#endif

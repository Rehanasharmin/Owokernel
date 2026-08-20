#include "drivers/driver.h"
#include "kernel.h"
#include "string.h"

#define MAX_DRIVERS 16
static Driver* registered_drivers[MAX_DRIVERS];
static int driver_count = 0;

void driver_register(Driver* driver) {
    if (driver_count < MAX_DRIVERS) {
        registered_drivers[driver_count++] = driver;
        if (driver->init) driver->init();
        kprintln("Driver Manager: Registered %s", driver->name);
    }
}

Driver* driver_get(const char* name) {
    for (int i = 0; i < driver_count; i++) {
        if (kstrcmp(registered_drivers[i]->name, name) == 0) {
            return registered_drivers[i];
        }
    }
    return NULL;
}

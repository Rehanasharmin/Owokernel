#include "drivers/driver.h"
#include "kernel.h"

#define MAX_DRIVERS 16
static Driver* registered_drivers[MAX_DRIVERS];
static int driver_count = 0;

void driver_register(Driver* driver) {
    if (driver_count < MAX_DRIVERS) {
        registered_drivers[driver_count++] = driver;
        kprintln("Driver Manager: Registered %s", driver->name);
    }
}

Driver* driver_get(const char* name) {
    for (int i = 0; i < driver_count; i++) {
        // Using a simple check instead of strcmp to keep it minimal
        // In a real kernel, we'd use a proper string compare.
        if (registered_drivers[i]->name == name) return registered_drivers[i];
    }
    return NULL;
}

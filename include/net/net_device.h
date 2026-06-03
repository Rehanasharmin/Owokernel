#ifndef NET_DEVICE_H
#define NET_DEVICE_H

#include "net_packet.h"
#include <stdint.h>

typedef struct NetDevice {
    const char* name;
    uint8_t mac[6];
    int (*init)();
    int (*send)(NetPacket* packet);
    int (*receive)(NetPacket** packet);
} NetDevice;

#endif

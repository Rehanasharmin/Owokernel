#ifndef NET_PACKET_H
#define NET_PACKET_H

#include <stdint.h>
#include <stddef.h>

#define MAX_PACKET_SIZE 1500

typedef struct NetPacket {
    uint8_t data[MAX_PACKET_SIZE];
    size_t length;
    struct NetPacket* next;
} NetPacket;

#endif

#include "net/net_stack.h"
#include "net/net_device.h"
#include "net/net_packet.h"
#include "heap.h"
#include "kernel.h"
#include "string.h"

extern NetDevice loopback_device;

static NetDevice* devices[4];
static int device_count = 0;

void net_init(void) {
    devices[device_count++] = &loopback_device;
    loopback_device.init();
    kprintln("Net: Networking stack initialized.");
}

static NetDevice* find_device(const char* name) {
    for (int i = 0; i < device_count; i++) {
        if (kstrcmp(devices[i]->name, name) == 0) return devices[i];
    }
    return NULL;
}

int net_send_packet(const char* dev_name, const uint8_t* data, size_t len) {
    NetDevice* dev = find_device(dev_name);
    if (!dev) return -1;

    NetPacket* pkt = (NetPacket*)kmalloc(sizeof(NetPacket));
    if (!pkt) return -1;

    if (len > MAX_PACKET_SIZE) len = MAX_PACKET_SIZE;
    kmemcpy(pkt->data, data, len);
    pkt->length = len;
    pkt->next = NULL;

    int res = dev->send(pkt);
    kfree(pkt);
    return res;
}

int net_recv_packet(const char* dev_name, uint8_t* buffer, size_t max_len) {
    NetDevice* dev = find_device(dev_name);
    if (!dev) return -1;

    NetPacket* pkt = NULL;
    if (dev->receive(&pkt) != 0) return -1;

    size_t len = pkt->length < max_len ? pkt->length : max_len;
    kmemcpy(buffer, pkt->data, len);
    kfree(pkt);
    return (int)len;
}

#include "net/net_device.h"
#include "net/net_packet.h"
#include "heap.h"
#include "kernel.h"

/* A simple queue for the loopback interface */
static NetPacket* rx_queue_head = NULL;
static NetPacket* rx_queue_tail = NULL;

static int lo_init() {
    kprintln("Net: Loopback interface (lo) initialized.");
    return 0;
}

static int lo_send(NetPacket* packet) {
    /* 
     * Loopback "send" is just adding the packet to the "receive" queue.
     */
    NetPacket* copy = (NetPacket*)kmalloc(sizeof(NetPacket));
    if (!copy) return -1;

    for (size_t i = 0; i < packet->length; i++) copy->data[i] = packet->data[i];
    copy->length = packet->length;
    copy->next = NULL;

    if (!rx_queue_head) {
        rx_queue_head = copy;
        rx_queue_tail = copy;
    } else {
        rx_queue_tail->next = copy;
        rx_queue_tail = copy;
    }

    return 0;
}

static int lo_receive(NetPacket** packet) {
    if (!rx_queue_head) return -1;

    *packet = rx_queue_head;
    rx_queue_head = rx_queue_head->next;
    if (!rx_queue_head) rx_queue_tail = NULL;

    return 0;
}

NetDevice loopback_device = {
    .name = "lo",
    .mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .init = lo_init,
    .send = lo_send,
    .receive = lo_receive
};

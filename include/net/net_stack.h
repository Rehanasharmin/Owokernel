#ifndef NET_STACK_H
#define NET_STACK_H

#include "net/net_device.h"
#include "net/net_packet.h"

void net_init();
int net_send_packet(const char* dev_name, const uint8_t* data, size_t len);
int net_recv_packet(const char* dev_name, uint8_t* buffer, size_t max_len);

#endif

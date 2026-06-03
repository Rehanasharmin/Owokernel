#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include <stddef.h>

void disk_init();
int disk_read_sector(uint32_t lba, uint8_t* buffer);

#endif

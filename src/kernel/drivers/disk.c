#include "drivers/disk.h"
#include "drivers/driver.h"
#include "io.h"
#include "kernel.h"

#define IDE_DATA 0x1F0
#define IDE_SEC_COUNT 0x1F2
#define IDE_LBA_LOW 0x1F3
#define IDE_LBA_MID 0x1F4
#define IDE_LBA_HIGH 0x1F5
#define IDE_DRIVE_HEAD 0x1F6
#define IDE_COMMAND 0x1F7
#define IDE_STATUS 0x1F7

int disk_read_sector(uint32_t lba, uint8_t* buffer) {
    outb(IDE_DRIVE_HEAD, 0xE0 | 0);
    outb(IDE_SEC_COUNT, 1);
    outb(IDE_LBA_LOW, (uint8_t)(lba & 0xFF));
    outb(IDE_LBA_MID, (uint8_t)((lba >> 8) & 0xFF));
    outb(IDE_LBA_HIGH, (uint8_t)((lba >> 16) & 0xFF));
    outb(IDE_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
    outb(IDE_COMMAND, 0x20);
    while (!(inb(IDE_STATUS) & 0x08));
    for (int i = 0; i < 256; i++) {
        buffer[i*2] = inb(IDE_DATA);
        buffer[i*2+1] = inb(IDE_DATA);
    }
    return 0;
}

static int do_disk_init() { return 0; }
static int do_disk_read(void* buf, size_t size) {
    return disk_read_sector(0, (uint8_t*)buf);
}

Driver disk_driver = {
    .name = "disk",
    .init = do_disk_init,
    .read = do_disk_read,
    .write = NULL
};

void disk_init() {
    driver_register(&disk_driver);
}

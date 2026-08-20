#include "vfs.h"
#include "heap.h"
#include "kernel.h"
#include "string.h"

static File file_table[MAX_FILES];

void vfs_init(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        file_table[i].open = false;
        file_table[i].inode = NULL;
        file_table[i].offset = 0;
    }
    ramfs_init();
}

int vfs_open(const char* path) {
    Inode* inode = ramfs_find_inode(path);
    if (!inode) inode = ramfs_create_inode(path);
    if (!inode) return -1;

    for (int i = 0; i < MAX_FILES; i++) {
        if (!file_table[i].open) {
            file_table[i].inode = inode;
            file_table[i].offset = 0;
            file_table[i].open = true;
            return i;
        }
    }
    return -1;
}

size_t vfs_write(int fd, const char* buffer, size_t size) {
    if (fd < 0 || fd >= MAX_FILES || !file_table[fd].open) return 0;

    File* file = &file_table[fd];
    Inode* inode = file->inode;
    size_t required_size = file->offset + size;

    if (inode->size < required_size) {
        uint8_t* new_data = (uint8_t*)kmalloc(required_size);
        if (!new_data) return 0;
        if (inode->data) {
            kmemcpy(new_data, inode->data, inode->size);
            kfree(inode->data);
        }
        inode->data = new_data;
        inode->size = required_size;
    }

    kmemcpy(inode->data + file->offset, buffer, size);
    file->offset += size;
    return size;
}

size_t vfs_read(int fd, char* buffer, size_t size) {
    if (fd < 0 || fd >= MAX_FILES || !file_table[fd].open) return 0;

    File* file = &file_table[fd];
    Inode* inode = file->inode;
    if (file->offset >= inode->size) return 0;

    size_t bytes_to_read = size;
    if (file->offset + size > inode->size) {
        bytes_to_read = inode->size - file->offset;
    }
    kmemcpy(buffer, inode->data + file->offset, bytes_to_read);
    file->offset += bytes_to_read;
    return bytes_to_read;
}

void vfs_close(int fd) {
    if (fd >= 0 && fd < MAX_FILES) {
        file_table[fd].open = false;
        file_table[fd].inode = NULL;
        file_table[fd].offset = 0;
    }
}

void vfs_list(void) {
    ramfs_list();
}

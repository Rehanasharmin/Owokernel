#include "vfs.h"
#include "heap.h"
#include "kernel.h"

/* Global File Table */
static File file_table[MAX_FILES];

void vfs_init() {
    for (int i = 0; i < MAX_FILES; i++) {
        file_table[i].open = false;
    }
    ramfs_init();
}

int vfs_open(const char* path) {
    Inode* inode = ramfs_find_inode(path);
    if (!inode) {
        // If it doesn't exist, create it (simulating O_CREAT)
        inode = ramfs_create_inode(path);
    }

    if (!inode) return -1;

    // Find a free slot in the file table
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

    // Ensure inode has enough buffer space
    size_t required_size = file->offset + size;
    if (inode->size < required_size) {
        uint8_t* new_data = (uint8_t*)kmalloc(required_size);
        if (!new_data) return 0;

        if (inode->data) {
            // Copy old data
            for (size_t i = 0; i < inode->size; i++) new_data[i] = inode->data[i];
            kfree(inode->data);
        }
        inode->data = new_data;
        inode->size = required_size;
    }

    // Copy data from buffer to inode
    for (size_t i = 0; i < size; i++) {
        inode->data[file->offset + i] = buffer[i];
    }

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

    for (size_t i = 0; i < bytes_to_read; i++) {
        buffer[i] = inode->data[file->offset + i];
    }

    file->offset += bytes_to_read;
    return bytes_to_read;
}

void vfs_close(int fd) {
    if (fd >= 0 && fd < MAX_FILES) {
        file_table[fd].open = false;
    }
}

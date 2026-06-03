#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_FILES 64
#define MAX_FILENAME 32

typedef struct Inode {
    char name[MAX_FILENAME];
    size_t size;
    uint8_t* data;
    bool is_directory;
} Inode;

typedef struct File {
    Inode* inode;
    size_t offset;
    bool open;
} File;

/* VFS API */
void vfs_init();
int vfs_open(const char* path);
size_t vfs_read(int fd, char* buffer, size_t size);
size_t vfs_write(int fd, const char* buffer, size_t size);
void vfs_close(int fd);

/* RAMFS Internal API */
void ramfs_init();
Inode* ramfs_find_inode(const char* name);
Inode* ramfs_create_inode(const char* name);

#endif

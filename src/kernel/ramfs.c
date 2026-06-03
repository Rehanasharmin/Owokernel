#include "vfs.h"
#include "heap.h"
#include "kernel.h"

static int custom_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

static Inode* inodes[MAX_FILES];
static int inode_count = 0;

void ramfs_init() {
    for (int i = 0; i < MAX_FILES; i++) {
        inodes[i] = NULL;
    }
    kprintln("RAMFS: Initialized.");
}

Inode* ramfs_find_inode(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (inodes[i] && custom_strcmp(inodes[i]->name, name) == 0) {
            return inodes[i];
        }
    }
    return NULL;
}

Inode* ramfs_create_inode(const char* name) {
    if (inode_count >= MAX_FILES) return NULL;

    Inode* inode = (Inode*)kmalloc(sizeof(Inode));
    if (!inode) return NULL;

    // Copy filename
    int i = 0;
    while (name[i] != '\0' && i < MAX_FILENAME - 1) {
        inode->name[i] = name[i];
        i++;
    }
    inode->name[i] = '\0';

    inode->size = 0;
    inode->data = NULL;
    inode->is_directory = false;

    // Find empty slot
    for (int j = 0; j < MAX_FILES; j++) {
        if (inodes[j] == NULL) {
            inodes[j] = inode;
            inode_count++;
            return inode;
        }
    }
    return NULL;
}

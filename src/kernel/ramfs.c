#include "vfs.h"
#include "heap.h"
#include "kernel.h"
#include "string.h"

static Inode* inodes[MAX_FILES];
static int inode_count = 0;

void ramfs_init(void) {
    for (int i = 0; i < MAX_FILES; i++) inodes[i] = NULL;
    inode_count = 0;
    kprintln("RAMFS: Initialized.");
}

Inode* ramfs_find_inode(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (inodes[i] && kstrcmp(inodes[i]->name, name) == 0) {
            return inodes[i];
        }
    }
    return NULL;
}

Inode* ramfs_create_inode(const char* name) {
    if (inode_count >= MAX_FILES) return NULL;

    Inode* inode = (Inode*)kmalloc(sizeof(Inode));
    if (!inode) return NULL;
    kmemset(inode, 0, sizeof(Inode));

    size_t i = 0;
    while (name[i] != '\0' && i < MAX_FILENAME - 1) {
        inode->name[i] = name[i];
        i++;
    }
    inode->name[i] = '\0';
    inode->size = 0;
    inode->data = NULL;
    inode->is_directory = false;

    for (int j = 0; j < MAX_FILES; j++) {
        if (inodes[j] == NULL) {
            inodes[j] = inode;
            inode_count++;
            return inode;
        }
    }
    kfree(inode);
    return NULL;
}

void ramfs_list(void) {
    kprintln("RAMFS files:");
    int any = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (inodes[i]) {
            kprintln("  %s (%d bytes)", inodes[i]->name, (int)inodes[i]->size);
            any = 1;
        }
    }
    if (!any) kprintln("  (empty)");
}

#pragma once
#include <stddef.h>
#include <stdint.h>

struct dirent {
    uint64_t d_fileno;
    unsigned short d_reclen;
    unsigned char d_type;
    unsigned char d_namlen;
    char d_name[255 + 1];
};

#define DT_UNKNOWN 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 6
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12

struct memfs_node;

typedef struct DIR {
    const struct memfs_node *node;
    size_t index;
} DIR;

DIR *opendir(const char *path);
struct dirent *readdir(DIR *dirp);
void rewinddir(DIR *dirp);
int closedir(DIR *dirp);

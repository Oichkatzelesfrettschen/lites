#pragma once

#include "../../include/auth.h"
#include "cap.h"
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct file_handle {
    int fd;
    unsigned long rights;
    const cap_t *owner;
    struct file_handle *next;
} file_handle_t;

file_handle_t *vfs_open(const cap_t *cap, const char *path, int flags);
ssize_t vfs_read(file_handle_t *h, void *buf, size_t len);
ssize_t vfs_write(file_handle_t *h, const void *buf, size_t len);
int vfs_close(file_handle_t *h);

#ifdef __cplusplus
}
#endif

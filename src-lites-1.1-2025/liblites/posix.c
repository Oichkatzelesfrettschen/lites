#include "../include/posix_fs.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

static file_handle_t *open_files;

file_handle_t *vfs_open(const cap_t *cap, const char *path, int flags) {
    unsigned long req = 0;
    if ((flags & O_WRONLY) || (flags & O_RDWR))
        req |= CAP_RIGHT_WRITE;
    if (!(flags & O_WRONLY))
        req |= CAP_RIGHT_READ;

    if ((cap->rights & req) != req || !authorize(cap, CAP_OP_OPEN, req)) {
        errno = EACCES;
        return NULL;
    }

    int fd = open(path, flags, 0644);
    if (fd < 0)
        return NULL;

    file_handle_t *h = malloc(sizeof(*h));
    if (!h) {
        close(fd);
        errno = ENOMEM;
        return NULL;
    }
    h->fd = fd;
    h->rights = req;
    h->owner = cap;
    h->next = open_files;
    open_files = h;
    return h;
}

ssize_t vfs_read(file_handle_t *h, void *buf, size_t len) {
    if (!h || !(h->rights & CAP_RIGHT_READ) || !authorize(h->owner, CAP_OP_READ, 0)) {
        errno = EACCES;
        return -1;
    }
    return read(h->fd, buf, len);
}

ssize_t vfs_write(file_handle_t *h, const void *buf, size_t len) {
    if (!h || !(h->rights & CAP_RIGHT_WRITE) || !authorize(h->owner, CAP_OP_WRITE, 0)) {
        errno = EACCES;
        return -1;
    }
    return write(h->fd, buf, len);
}

int vfs_close(file_handle_t *h) {
    if (!h || !authorize(h->owner, CAP_OP_CLOSE, 0)) {
        errno = EACCES;
        return -1;
    }
    file_handle_t **p = &open_files;
    while (*p && *p != h)
        p = &(*p)->next;
    if (*p)
        *p = h->next;
    int r = close(h->fd);
    free(h);
    return r;
}

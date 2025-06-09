/* See LICENSE_MAP for license and origin information */
#include "posix.h"
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/** Structure describing a named FIFO created by @ref mkfifo. */
struct fifo_entry {
    char *path;               /**< Path backing the FIFO. */
    int fd[2];                /**< Read/write descriptor pair. */
    struct fifo_entry *next;  /**< Next entry in the list. */
};

/** Head of the linked list of FIFOs. */
static struct fifo_entry *fifo_list;

/**
 * Locate a FIFO entry by path.
 *
 * @param path Absolute or relative FIFO path.
 * @return Pointer to the entry or NULL if not found.
 */
static struct fifo_entry *find_fifo(const char *path) {
    for (struct fifo_entry *f = fifo_list; f; f = f->next) {
        if (strcmp(f->path, path) == 0)
            return f;
    }
    return NULL;
}

/**
 * Create an in-memory FIFO for the given path.
 *
 * This implementation emulates FIFOs by maintaining an internal
 * table of pipes. The mode argument is presently ignored.
 *
 * @param path Path identifying the FIFO.
 * @param mode Permission bits (unused).
 * @return 0 on success or -1 and errno set on failure.
 */
int mkfifo(const char *path, mode_t mode) {
    (void)mode;
    if (find_fifo(path)) {
        errno = EEXIST;
        return -1;
    }
    struct fifo_entry *f = malloc(sizeof(*f));
    if (!f) {
        errno = ENOMEM;
        return -1;
    }
    if (pipe(f->fd) < 0) {
        free(f);
        return -1;
    }
    f->path = strdup(path);
    if (!f->path) {
        close(f->fd[0]);
        close(f->fd[1]);
        free(f);
        errno = ENOMEM;
        return -1;
    }
    f->next = fifo_list;
    fifo_list = f;
    return 0;
}

enum fd_kind { FD_NONE, FD_FILE, FD_FIFO_R, FD_FIFO_W, FD_FIFO_RW };
struct fd_entry {
    enum fd_kind kind;
    int rfd;
    int wfd;
};

static struct fd_entry *fd_table;
static size_t fd_table_sz;

/**
 * Ensure the file-descriptor table is large enough for the given index.
 *
 * @param fd Index into the descriptor table that must be valid.
 */
static void ensure_fd(size_t fd) {
    if (fd >= fd_table_sz) {
        size_t new_sz = fd + 16;
        struct fd_entry *n = realloc(fd_table, new_sz * sizeof(*n));
        if (!n)
            return;
        for (size_t i = fd_table_sz; i < new_sz; ++i)
            n[i].kind = FD_NONE;
        fd_table = n;
        fd_table_sz = new_sz;
    }
}

/**
 * Open a file or emulated FIFO.
 *
 * When the path refers to a FIFO created by @ref mkfifo this function
 * duplicates the pipe descriptors and records them in an internal table.
 * Otherwise it falls back to the openat system call.
 *
 * @param path  File system path.
 * @param flags Flags as accepted by open(2).
 * @param ...   Optional mode_t when O_CREAT is supplied.
 * @return New file descriptor or -1 on error.
 */
int open(const char *path, int flags, ...) {
    mode_t mode = 0;
    if (flags & O_CREAT) {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, int);
        va_end(ap);
    }
    struct fifo_entry *f = find_fifo(path);
    if (f) {
        if ((flags & O_RDWR) == O_RDWR) {
            int r = dup(f->fd[0]);
            if (r < 0)
                return -1;
            int w = dup(f->fd[1]);
            if (w < 0) {
                close(r);
                return -1;
            }
            ensure_fd(r);
            fd_table[r].kind = FD_FIFO_RW;
            fd_table[r].rfd = r;
            fd_table[r].wfd = w;
            return r;
        } else if (flags & O_WRONLY) {
            int fd = dup(f->fd[1]);
            if (fd < 0)
                return -1;
            ensure_fd(fd);
            fd_table[fd].kind = FD_FIFO_W;
            fd_table[fd].rfd = fd;
            fd_table[fd].wfd = fd;
            return fd;
        } else {
            int fd = dup(f->fd[0]);
            if (fd < 0)
                return -1;
            ensure_fd(fd);
            fd_table[fd].kind = FD_FIFO_R;
            fd_table[fd].rfd = fd;
            fd_table[fd].wfd = fd;
            return fd;
        }
    }
    int fd = syscall(SYS_openat, AT_FDCWD, path, flags, mode);
    if (fd >= 0) {
        ensure_fd(fd);
        fd_table[fd].kind = FD_FILE;
        fd_table[fd].rfd = fd;
        fd_table[fd].wfd = fd;
    }
    return fd;
}

/**
 * Wrapper around read(2) that understands FIFO descriptors.
 *
 * @param fd    Descriptor to read from.
 * @param buf   Buffer to fill.
 * @param count Number of bytes to read.
 * @return Bytes read or -1 on failure.
 */
ssize_t read(int fd, void *buf, size_t count) {
    if ((size_t)fd < fd_table_sz) {
        struct fd_entry *e = &fd_table[fd];
        if (e->kind == FD_FIFO_W) {
            errno = EBADF;
            return -1;
        } else if (e->kind == FD_FIFO_RW) {
            return syscall(SYS_read, e->rfd, buf, count);
        }
    }
    return syscall(SYS_read, fd, buf, count);
}

/**
 * Wrapper around write(2) that understands FIFO descriptors.
 *
 * @param fd    Descriptor to write to.
 * @param buf   Data buffer.
 * @param count Number of bytes to write.
 * @return Bytes written or -1 on failure.
 */
ssize_t write(int fd, const void *buf, size_t count) {
    if ((size_t)fd < fd_table_sz) {
        struct fd_entry *e = &fd_table[fd];
        if (e->kind == FD_FIFO_R) {
            errno = EBADF;
            return -1;
        } else if (e->kind == FD_FIFO_RW) {
            return syscall(SYS_write, e->wfd, buf, count);
        }
    }
    return syscall(SYS_write, fd, buf, count);
}

/**
 * Duplicate a file descriptor, correctly handling emulated FIFOs.
 *
 * @param fd Descriptor to duplicate.
 * @return New descriptor or -1 on error.
 */
int dup(int fd) {
    if ((size_t)fd < fd_table_sz) {
        struct fd_entry *e = &fd_table[fd];
        if (e->kind == FD_FIFO_RW) {
            int r = syscall(SYS_dup, e->rfd);
            if (r < 0)
                return -1;
            int w = syscall(SYS_dup, e->wfd);
            if (w < 0) {
                syscall(SYS_close, r);
                return -1;
            }
            ensure_fd(r);
            fd_table[r].kind = FD_FIFO_RW;
            fd_table[r].rfd = r;
            fd_table[r].wfd = w;
            return r;
        } else if (e->kind == FD_FIFO_R || e->kind == FD_FIFO_W || e->kind == FD_FILE) {
            int n = syscall(SYS_dup, fd);
            if (n < 0)
                return -1;
            ensure_fd(n);
            fd_table[n] = *e;
            return n;
        }
    }
    return syscall(SYS_dup, fd);
}

/**
 * Close a file descriptor and clean up any associated FIFO state.
 *
 * @param fd Descriptor to close.
 * @return 0 on success or -1 on error.
 */
int close(int fd) {
    if ((size_t)fd < fd_table_sz) {
        struct fd_entry *e = &fd_table[fd];
        if (e->kind == FD_FIFO_RW) {
            syscall(SYS_close, e->wfd);
            syscall(SYS_close, e->rfd);
            e->kind = FD_NONE;
            return 0;
        } else if (e->kind != FD_NONE) {
            e->kind = FD_NONE;
            return syscall(SYS_close, fd);
        }
    }
    return syscall(SYS_close, fd);
}

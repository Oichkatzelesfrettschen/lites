#pragma once
#include <fcntl.h>
#include <stddef.h>
#ifdef open
#undef open
#endif
#ifdef read
#undef read
#endif
#ifdef write
#undef write
#endif
#ifdef close
#undef close
#endif
#ifdef dup
#undef dup
#endif
#include <unistd.h>

int mkfifo(const char *path, mode_t mode);
int open(const char *path, int flags, ...);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int dup(int fd);
int close(int fd);

#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <stddef.h>

int px_setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);
int px_getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen);

int sockaddr_from_str(const char *str, struct sockaddr_storage *out);
const char *sockaddr_to_str(const struct sockaddr *addr, char *buf, size_t buflen);


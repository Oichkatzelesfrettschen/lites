#include "../include/posix_net.h"
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int px_setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen) {
    int ret;
    do {
        ret = setsockopt(fd, level, optname, optval, optlen);
    } while (ret < 0 && errno == EINTR);
    return ret;
}

int px_getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen) {
    int ret;
    do {
        ret = getsockopt(fd, level, optname, optval, optlen);
    } while (ret < 0 && errno == EINTR);
    return ret;
}

int sockaddr_from_str(const char *str, struct sockaddr_storage *out) {
    memset(out, 0, sizeof(*out));
    if (inet_pton(AF_INET, str, &((struct sockaddr_in*)out)->sin_addr) == 1) {
        out->ss_family = AF_INET;
        return 0;
    }
    if (inet_pton(AF_INET6, str, &((struct sockaddr_in6*)out)->sin6_addr) == 1) {
        out->ss_family = AF_INET6;
        return 0;
    }
    return -1;
}

const char *sockaddr_to_str(const struct sockaddr *addr, char *buf, size_t buflen) {
    int af = addr->sa_family;
    const void *src = NULL;
    if (af == AF_INET) {
        src = &((const struct sockaddr_in*)addr)->sin_addr;
    } else if (af == AF_INET6) {
        src = &((const struct sockaddr_in6*)addr)->sin6_addr;
    } else {
        errno = EAFNOSUPPORT;
        return NULL;
    }
    return inet_ntop(af, src, buf, buflen);
}


# POSIX Networking Helpers

A lightweight POSIX compatibility library lives under `posix/`.  It provides
a few wrappers and helpers for socket programming.

## Socket option wrappers

```c
int px_setsockopt(int fd, int level, int optname,
                  const void *optval, socklen_t optlen);
int px_getsockopt(int fd, int level, int optname,
                  void *optval, socklen_t *optlen);
```

These functions behave like `setsockopt(2)` and `getsockopt(2)` but
retry automatically if interrupted by `EINTR`.

## Address conversion

```c
int sockaddr_from_str(const char *str, struct sockaddr_storage *out);
const char *sockaddr_to_str(const struct sockaddr *addr,
                            char *buf, size_t buflen);
```

`sockaddr_from_str` parses an IPv4 or IPv6 textual address into a
`struct sockaddr_storage`.  `sockaddr_to_str` performs the reverse
conversion.  Both return `NULL` or `-1` on failure with `errno` set.

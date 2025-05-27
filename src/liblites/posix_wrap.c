#include "posix_wrap.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/uio.h>

int lx_link(const char *oldpath, const char *newpath)
{
    int r;
    do {
        r = link(oldpath, newpath);
    } while (r < 0 && errno == EINTR);
    return r;
}

int lx_unlink(const char *path)
{
    int r;
    do {
        r = unlink(path);
    } while (r < 0 && errno == EINTR);
    return r;
}

char *lx_getcwd(char *buf, size_t size)
{
    char *r;
    do {
        r = getcwd(buf, size);
    } while (!r && errno == EINTR);
    return r;
}

int lx_chdir(const char *path)
{
    int r;
    do {
        r = chdir(path);
    } while (r < 0 && errno == EINTR);
    return r;
}

static const char *find_in_path(const char *file, char *buf, size_t buflen, const char *path)
{
    const char *start = path;
    while (start && *start) {
        const char *colon = strchr(start, ':');
        size_t len = colon ? (size_t)(colon - start) : strlen(start);
        if (len + 1 + strlen(file) + 1 < buflen) {
            memcpy(buf, start, len);
            buf[len] = '/';
            strcpy(buf + len + 1, file);
            if (access(buf, X_OK) == 0)
                return buf;
        }
        if (!colon)
            break;
        start = colon + 1;
    }
    return NULL;
}

static const char *env_lookup(char *const envp[], const char *name)
{
    size_t nlen = strlen(name);
    if (envp) {
        for (char *const *p = envp; *p; ++p) {
            if (strncmp(*p, name, nlen) == 0 && (*p)[nlen] == '=')
                return *p + nlen + 1;
        }
    }
    const char *val = getenv(name);
    return val;
}

int lx_execvep(const char *file, char *const argv[], char *const envp[])
{
    if (strchr(file, '/'))
        return execve(file, argv, envp);

    const char *path = env_lookup(envp, "PATH");
    if (!path)
        path = "/bin:/usr/bin";

    char buf[PATH_MAX];
    const char *full = find_in_path(file, buf, sizeof(buf), path);
    if (full)
        return execve(full, argv, envp);
    errno = ENOENT;
    return -1;
}

pid_t lx_waitpid(pid_t pid, int *status, int options)
{
    pid_t r;
    do {
        r = waitpid(pid, status, options);
    } while (r < 0 && errno == EINTR);
    return r;
}

pid_t lx_wait(int *status)
{
    return lx_waitpid(-1, status, 0);
}

static int set_cloexec(int fd)
{
    int flags = fcntl(fd, F_GETFD);
    if (flags < 0)
        return -1;
    return fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
}

int lx_socket_cloexec(int domain, int type, int protocol)
{
#ifdef SOCK_CLOEXEC
    int fd;
    do {
        fd = socket(domain, type | SOCK_CLOEXEC, protocol);
    } while (fd < 0 && errno == EINTR);
    if (fd >= 0 || errno != EINVAL)
        return fd;
#endif
    int fd;
    do {
        fd = socket(domain, type, protocol);
    } while (fd < 0 && errno == EINTR);
    if (fd >= 0)
        set_cloexec(fd);
    return fd;
}

int lx_accept_cloexec(int sockfd, struct sockaddr *addr, socklen_t *len)
{
#ifdef SOCK_CLOEXEC
    int fd;
    do {
        fd = accept4(sockfd, addr, len, SOCK_CLOEXEC);
    } while (fd < 0 && errno == EINTR);
    if (fd >= 0 || errno != ENOSYS)
        return fd;
#endif
    int fd;
    do {
        fd = accept(sockfd, addr, len);
    } while (fd < 0 && errno == EINTR);
    if (fd >= 0)
        set_cloexec(fd);
    return fd;
}

int lx_send_fd(int sockfd, int fd)
{
    struct msghdr msg = {0};
    char buf = 'x';
    struct iovec io = {&buf, 1};
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    char cbuf[CMSG_SPACE(sizeof(int))];
    msg.msg_control = cbuf;
    msg.msg_controllen = sizeof(cbuf);
    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    memcpy(CMSG_DATA(cmsg), &fd, sizeof(int));
    ssize_t n;
    do {
        n = sendmsg(sockfd, &msg, 0);
    } while (n < 0 && errno == EINTR);
    return n == 1 ? 0 : -1;
}

int lx_recv_fd(int sockfd)
{
    struct msghdr msg = {0};
    char buf;
    struct iovec io = {&buf, 1};
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    char cbuf[CMSG_SPACE(sizeof(int))];
    msg.msg_control = cbuf;
    msg.msg_controllen = sizeof(cbuf);
    ssize_t n;
    do {
        n = recvmsg(sockfd, &msg, 0);
    } while (n < 0 && errno == EINTR);
    if (n <= 0)
        return -1;
    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    if (!cmsg || cmsg->cmsg_level != SOL_SOCKET || cmsg->cmsg_type != SCM_RIGHTS)
        return -1;
    int fd;
    memcpy(&fd, CMSG_DATA(cmsg), sizeof(int));
    return fd;
}


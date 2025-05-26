# POSIX helpers

A small collection of convenience wrappers lives in
`liblites/posix_wrap.c` and the associated header
`include/posix_wrap.h` within the project source tree.

```c
int lx_link(const char *oldpath, const char *newpath);
int lx_unlink(const char *path);
char *lx_getcwd(char *buf, size_t size);
int lx_chdir(const char *path);
int lx_execvep(const char *file, char *const argv[], char *const envp[]);

pid_t lx_waitpid(pid_t pid, int *status, int options);
pid_t lx_wait(int *status);

int lx_socket_cloexec(int domain, int type, int protocol);
int lx_accept_cloexec(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int lx_send_fd(int sockfd, int fd);
int lx_recv_fd(int sockfd);
```

`lx_link`, `lx_unlink`, `lx_getcwd` and `lx_chdir` simply retry the
underlying system call when it fails with `EINTR`.

`lx_execvep` behaves like `execvpe`: when the file name does not contain a
slash it searches the `PATH` environment variable (either from `envp` or
from the current process) for an executable and then calls `execve`.

`lx_waitpid` and `lx_wait` wrap the standard waiting functions and ignore
`EINTR` interruptions.

The socket helpers create or accept descriptors with the close-on-exec
flag and provide a simple way to send and receive file descriptors over a
UNIX domain socket.

## Example

```c
#include "posix_wrap.h"

int main(void) {
    char *args[] = {"echo", "hi", NULL};
    char *env[] = {"PATH=/bin:/usr/bin", NULL};
    lx_execvep("echo", args, env);
    return 0; /* only reached on error */
}
```

Compile manually with:

```sh
cc -I "$LITES_SRC_DIR/include" \
    "$LITES_SRC_DIR/liblites/posix_wrap.c" bin/posix-demo.c -o posix-demo
```

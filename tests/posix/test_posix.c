#include "../../src-lites-1.1-2025/include/posix_wrap.h"
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main(void) {
    char file[] = "tfileXXXXXX";
    int fd = mkstemp(file);
    assert(fd >= 0);
    close(fd);

    assert(lx_link(file, "tlink") == 0);
    assert(lx_unlink(file) == 0);
    assert(access("tlink", F_OK) == 0);

    char buf[256];
    assert(lx_getcwd(buf, sizeof(buf)));
    assert(lx_chdir(".") == 0);

    pid_t pid = fork();
    assert(pid >= 0);
    if (pid == 0) {
        char *args[] = {"true", NULL};
        char *env[] = {"PATH=/bin:/usr/bin", NULL};
        lx_execvep("true", args, env);
        _exit(1);
    }
    assert(lx_waitpid(pid, NULL, 0) == pid);

    assert(lx_unlink("tlink") == 0);

    int sv[2];
    assert(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == 0);
    int dupfd = dup(sv[0]);
    assert(dupfd >= 0);
    assert(lx_send_fd(sv[0], dupfd) == 0);
    close(dupfd);
    int recvfd = lx_recv_fd(sv[1]);
    assert(recvfd >= 0);
    close(recvfd);
    close(sv[0]);
    close(sv[1]);

    return 0;
}

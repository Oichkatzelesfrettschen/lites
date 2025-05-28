#include "posix.h"
#include <assert.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int fds[2];
    assert(pipe(fds) == 0);
    pid_t pid = fork();
    assert(pid >= 0);
    if (pid == 0) {
        close(fds[0]);
        const char msg[] = "hello";
        write(fds[1], msg, sizeof(msg));
        close(fds[1]);
        _exit(0);
    }
    close(fds[1]);
    char buf[6] = {0};
    read(fds[0], buf, sizeof(buf));
    assert(buf[0] == 'h');
    close(fds[0]);
    posix_waitpid(pid, NULL, 0);
    return 0;
}

/**
 * Lites repository license applies to this file; see the LICENSE file
 * in the project root for details.
 */

#include "posix_wrap.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    char cwd[256];
    lx_getcwd(cwd, sizeof(cwd));
    printf("cwd: %s\n", cwd);

    int fd = open("demo.tmp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    write(fd, "hello", 5);
    close(fd);

    if (lx_link("demo.tmp", "demo.lnk") != 0) {
        perror("link");
    }

    if (lx_unlink("demo.tmp") != 0) {
        perror("unlink");
    }

    char *args[] = {"echo", "done", NULL};
    char *env[] = {"PATH=/bin:/usr/bin", NULL};
    pid_t pid = fork();
    if (pid == 0) {
        lx_execvep("echo", args, env);
        _exit(1);
    }
    lx_waitpid(pid, NULL, 0);

    lx_unlink("demo.lnk");
    return 0;
}

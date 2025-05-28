#include "posix.h"
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

pid_t posix_spawn(const char *path, char *const argv[], char *const envp[]) {
    pid_t pid = fork();
    if (pid == 0) {
        posix_execve(path, argv, envp);
        _exit(127);
    }
    return pid;
}

int posix_execve(const char *path, char *const argv[], char *const envp[]) {
    char **use_env = (char **)envp;
    if (!use_env)
        use_env = environ;
    return execve(path, argv, use_env);
}

pid_t posix_waitpid(pid_t pid, int *status, int options) {
    pid_t ret;
    do {
        ret = waitpid(pid, status, options);
    } while (ret == -1 && errno == EINTR);
    return ret;
}

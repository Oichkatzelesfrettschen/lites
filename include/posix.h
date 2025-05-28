#pragma once
#include <sys/types.h>

pid_t posix_spawn(const char *path, char *const argv[], char *const envp[]);
int posix_execve(const char *path, char *const argv[], char *const envp[]);
pid_t posix_waitpid(pid_t pid, int *status, int options);


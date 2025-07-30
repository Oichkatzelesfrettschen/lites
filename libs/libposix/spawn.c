#include "posix.h"
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

/**
 * @brief Spawn a child process running the specified program.
 *
 * Forks the current process and executes @p path with the given arguments
 * and environment in the child. The parent receives the PID of the new
 * process.
 *
 * @param path Executable to run.
 * @param argv Null-terminated argument vector.
 * @param envp Null-terminated environment vector or NULL for @c environ.
 * @return PID of the child on success, -1 on failure.
 */
pid_t posix_spawn(const char *path, char *const argv[], char *const envp[]) {
    pid_t pid = fork();
    if (pid == 0) {
        posix_execve(path, argv, envp);
        _exit(127);
    }
    return pid;
}

/**
 * @brief Execute a program, replacing the current process image.
 *
 * This is a thin wrapper around execve that falls back to the global
 * @c environ array when @p envp is NULL.
 *
 * @param path Executable path.
 * @param argv Argument vector.
 * @param envp Environment vector or NULL.
 * @return Only returns on failure with -1 and errno set.
 */
int posix_execve(const char *path,
                 char *const argv[],  // NOLINT(bugprone-easily-swappable-parameters)
                 char *const envp[]) {
    char *const *use_env = envp ? envp : (char *const *)environ;
    return execve(path, argv, use_env);
}

/**
 * @brief Wait for a specific child process to change state.
 *
 * Retries the waitpid call if interrupted by a signal.
 *
 * @param pid     Child process ID.
 * @param status  Location to store the exit status.
 * @param options Flags controlling waitpid behaviour.
 * @return PID of the child on success or -1 on error.
 */
pid_t posix_waitpid(pid_t pid, int *status, int options) {
    pid_t ret;
    do {
        ret = waitpid(pid, status, options);
    } while (ret == -1 && errno == EINTR);
    return ret;
}

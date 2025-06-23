#ifndef LITES_POSIX_WRAP_H
#define LITES_POSIX_WRAP_H

#include <sys/types.h>
#include <sys/socket.h>

/**
 * @brief Retry link(2) on EINTR.
 *
 * @param oldpath Path to existing file.
 * @param newpath Path of the new link.
 * @return 0 on success, -1 on error.
 */
int lx_link(const char *oldpath, const char *newpath);

/**
 * @brief Retry unlink(2) on EINTR.
 *
 * @param path File to remove.
 * @return 0 on success, -1 on error.
 */
int lx_unlink(const char *path);

/**
 * @brief Retry getcwd(3) on EINTR.
 *
 * @param buf  Buffer to store the directory path.
 * @param size Buffer size in bytes.
 * @return buf on success, NULL on error.
 */
char *lx_getcwd(char *buf, size_t size);

/**
 * @brief Retry chdir(2) on EINTR.
 *
 * @param path Directory to change into.
 * @return 0 on success, -1 on error.
 */
int lx_chdir(const char *path);

/**
 * @brief Search PATH and execve the first match.
 *
 * @param file Executable name.
 * @param argv Argument vector.
 * @param envp Environment array.
 * @return 0 on success, -1 on failure.
 */
int lx_execvep(const char *file, char *const argv[], char *const envp[]);

/**
 * @brief Retry waitpid(2) on EINTR.
 */
pid_t lx_waitpid(pid_t pid, int *status, int options);

/** @brief Retry wait(2) on EINTR. */
pid_t lx_wait(int *status);

/** @brief Create a socket with FD_CLOEXEC set when possible. */
int lx_socket_cloexec(int domain, int type, int protocol);

/**
 * @brief Accept a connection and set FD_CLOEXEC.
 */
int lx_accept_cloexec(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

/** @brief Send a file descriptor over a UNIX domain socket. */
int lx_send_fd(int sockfd, int fd);

/** @brief Receive a file descriptor from a UNIX domain socket. */
int lx_recv_fd(int sockfd);

#endif /* LITES_POSIX_WRAP_H */

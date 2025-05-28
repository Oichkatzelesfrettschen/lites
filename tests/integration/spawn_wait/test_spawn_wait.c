#include "../../include/posix.h"
#include <assert.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    char *argv[] = {"./tests/spawn_wait/child", NULL};
    pid_t pid = posix_spawn("./tests/spawn_wait/child", argv, NULL);
    assert(pid > 0);
    int status = 0;
    pid_t w = posix_waitpid(pid, &status, 0);
    assert(w == pid);
    assert(WIFEXITED(status));
    assert(WEXITSTATUS(status) == 0);
    return 0;
}

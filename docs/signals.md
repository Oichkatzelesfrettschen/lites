# Signal helpers

The modernized tree provides thin wrappers around common signal calls.
They behave like their POSIX counterparts but are grouped under a
single header for convenience.

```c
int lites_sigaction(int signum, const struct sigaction *act,
                    struct sigaction *oldact);
int lites_sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
int lites_killpg(pid_t pgid, int sig);
```

`lites_sigaction` installs a handler for `signum` and optionally returns the
previous action. `lites_sigprocmask` adjusts the calling thread's blocked
signals. `lites_killpg` sends `sig` to every process in the specified group
and falls back to `kill(-pgid, sig)` when `killpg` is unavailable.

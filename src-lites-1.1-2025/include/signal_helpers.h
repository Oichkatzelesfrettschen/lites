#pragma once

#include <signal.h>
#include <sys/types.h>

int lites_sigaction(int signum, const struct sigaction *act,
                    struct sigaction *oldact);
int lites_sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
int lites_killpg(pid_t pgid, int sig);


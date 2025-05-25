#include "signal_helpers.h"

#include <signal.h>
#include <unistd.h>

int lites_sigaction(int signum, const struct sigaction *act,
                    struct sigaction *oldact)
{
    return sigaction(signum, act, oldact);
}

int lites_sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
    return sigprocmask(how, set, oldset);
}

int lites_killpg(pid_t pgid, int sig)
{
#ifdef HAVE_KILLPG
    return killpg(pgid, sig);
#else
    return kill(-pgid, sig);
#endif
}


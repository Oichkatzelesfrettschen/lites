#include "../../src-lites-1.1-2025/include/signal_helpers.h"
#include <assert.h>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t got;

static void handler(int sig) { got = sig; }

int main(void) {
    struct sigaction sa = {0};
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    assert(lites_sigaction(SIGUSR1, &sa, NULL) == 0);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    assert(lites_sigprocmask(SIG_BLOCK, &mask, NULL) == 0);

    assert(lites_killpg(getpgrp(), SIGUSR1) == 0);
    assert(got == 0);

    assert(lites_sigprocmask(SIG_UNBLOCK, &mask, NULL) == 0);
    assert(got == SIGUSR1);

    return 0;
}

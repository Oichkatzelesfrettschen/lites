#include "../../include/timer.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    timer_init();
    pid_t pid = getpid();
    timer_reset(pid);

    struct timespec ts = {0, 5000000};
    uint64_t before = timer_get_total_ns(pid);
    k_nanosleep(&ts, NULL);
    uint64_t after = timer_get_total_ns(pid);
    assert(after >= before + 1000000);

    timer_reset(pid);
    assert(timer_get_total_ns(pid) == 0);
    (void)after;
    printf("timer test passed\n");
    return 0;
}

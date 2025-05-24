#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../../include/vm.h"

/*
 * Minimal user level pager. It listens on a socket passed as the first
 * argument. For every fault request it simply acknowledges and provides
 * zero filled pages implicitly.
 */
int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s <fd>\n", argv[0]);
        return 1;
    }

    int fd = atoi(argv[1]);
    pf_info_t info;
    char ack = 0;
    while (read(fd, &info, sizeof(info)) == (ssize_t)sizeof(info)) {
        /* In a real pager we would map or create a page here. */
        write(fd, &ack, 1);
    }
    return 0;
}

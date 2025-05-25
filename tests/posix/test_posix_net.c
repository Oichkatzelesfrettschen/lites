#include "../../include/posix_net.h"
#include <assert.h>
#include <string.h>
#include <unistd.h>

static void test_sockopt(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(fd >= 0);
    int val = 1;
    assert(px_setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == 0);
    val = 0;
    socklen_t len = sizeof(val);
    assert(px_getsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, &len) == 0);
    assert(val == 1);
    close(fd);
}

static void test_addr_conversion(void) {
    struct sockaddr_storage ss;
    assert(sockaddr_from_str("127.0.0.1", &ss) == 0);
    char buf[64];
    const char *out = sockaddr_to_str((struct sockaddr *)&ss, buf, sizeof(buf));
    assert(out);
    assert(strcmp(out, "127.0.0.1") == 0);
}

int main(void) {
    test_sockopt();
    test_addr_conversion();
    return 0;
}

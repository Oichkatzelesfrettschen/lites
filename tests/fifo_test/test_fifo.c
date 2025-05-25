#include "../../src-lites-1.1-2025/include/posix.h"
#include <assert.h>
#include <string.h>

int main(void) {
    assert(mkfifo("myfifo", 0600) == 0);
    int fdw = open("myfifo", O_WRONLY);
    assert(fdw >= 0);
    int fdr = open("myfifo", O_RDONLY);
    assert(fdr >= 0);
    const char msg[] = "hello";
    assert(write(fdw, msg, sizeof(msg)) == sizeof(msg));
    char buf[16];
    assert(read(fdr, buf, sizeof(msg)) == sizeof(msg));
    assert(memcmp(buf, msg, sizeof(msg)) == 0);
    int dupfd = dup(fdr);
    assert(dupfd >= 0);
    assert(close(fdr) == 0);
    assert(close(fdw) == 0);
    assert(close(dupfd) == 0);
    return 0;
}

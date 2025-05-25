CC ?= gcc
CFLAGS += -std=c23 -Wall -Wextra -Werror
CPPFLAGS += -I../../include

all: test_vfs

.PHONY: all clean

test_vfs: test_vfs.c ../../src-lites-1.1-2025/liblites/posix.c \
../../kern/auth.c ../../kern/audit.c \
../../src-lites-1.1-2025/server/kern/cap.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_vfs

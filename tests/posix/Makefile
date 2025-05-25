CC ?= gcc
CFLAGS += -std=c23 -Wall -Wextra -Werror
CPPFLAGS += -I../../src-lites-1.1-2025/include

all: test_posix

.PHONY: all clean

test_posix: test_posix.c ../../src-lites-1.1-2025/liblites/posix_wrap.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)


clean:
	rm -f test_posix

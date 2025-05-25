CC ?= gcc
CFLAGS += -std=c23 -Wall -Wextra -Werror
CPPFLAGS += -I../../include

all: test_posix_net

.PHONY: all clean

test_posix_net: test_posix_net.c ../../posix/net.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_posix_net

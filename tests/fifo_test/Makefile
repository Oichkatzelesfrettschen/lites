CC ?= clang
CFLAGS += -std=c23 -Wall -Wextra -Werror
CPPFLAGS += -I../../src-lites-1.1-2025/include

all: test_fifo

.PHONY: all clean

test_fifo: test_fifo.c ../../src-lites-1.1-2025/libposix/posix.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_fifo

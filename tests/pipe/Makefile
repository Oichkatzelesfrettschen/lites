CC ?= clang
CFLAGS += -std=gnu23 -Wall -Wextra -Werror
CPPFLAGS += -I../../include

all: test_pipe

.PHONY: all clean

test_pipe: test_pipe.c ../../posix.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_pipe

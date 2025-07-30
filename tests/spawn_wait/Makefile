CC ?= clang
CFLAGS += -std=gnu23 -Wall -Wextra -Werror
CPPFLAGS += -I../../include

all: test_spawn_wait child

.PHONY: all clean

test_spawn_wait: test_spawn_wait.c \
	../../libs/libposix/posix.c ../../libs/libposix/spawn.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

child: child.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

clean:
	rm -f test_spawn_wait child

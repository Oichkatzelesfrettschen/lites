CC ?= gcc
# Additional compile flags from the top-level build system are appended.
CFLAGS += -std=c23 -Wall -Wextra -Werror
CPPFLAGS += -I../../include

all: test_timer

.PHONY: all clean

test_timer: test_timer.c ../../kern/timer.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_timer

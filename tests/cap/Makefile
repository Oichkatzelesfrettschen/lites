CC ?= gcc
# Additional compile flags from the top-level build system are appended.
CFLAGS += -std=c23 -Wall -Wextra -Werror
CPPFLAGS += -I../../include

all: test_cap


.PHONY: all clean

test_cap: test_cap.c ../../src-lites-1.1-2025/server/kern/cap.c ../../kern/auth.c ../../kern/audit.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_cap

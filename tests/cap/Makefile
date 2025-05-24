CC ?= gcc
CFLAGS ?= -std=gnu2x -Wall -Wextra

all: test_cap


.PHONY: all clean

test_cap: test_cap.c ../../src-lites-1.1-2025/server/kern/cap.c ../../kern/auth.c ../../kern/audit.c
	$(CC) $(CFLAGS) -I../../include $^ -o $@

clean:
		rm -f test_cap

ifeq ($(origin CC),default)
CC := $(shell command -v clang >/dev/null 2>&1 && echo clang || echo gcc)
endif
CFLAGS ?= -std=c23 -Wall -Werror

all: test_cap


.PHONY: all clean

test_cap: test_cap.c ../../src-lites-1.1-2025/server/kern/cap.c ../../kern/auth.c ../../kern/audit.c
	$(CC) $(CFLAGS) -I../../include $^ -o $@

clean:
		rm -f test_cap

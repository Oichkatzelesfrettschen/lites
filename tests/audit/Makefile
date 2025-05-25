ifeq ($(origin CC),default)
CC := $(shell command -v clang >/dev/null 2>&1 && echo clang || echo gcc)
endif
CFLAGS ?= -std=c23 -Wall -Werror

all: test_audit


.PHONY: all clean

test_audit: test_audit.c ../../kern/auth.c ../../kern/audit.c
	$(CC) $(CFLAGS) -I../../include $^ -o $@

clean:
		rm -f test_audit

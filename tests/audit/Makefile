CC ?= gcc
CFLAGS ?= -std=gnu2x -Wall -Wextra

all: test_audit


.PHONY: all clean

test_audit: test_audit.c ../../kern/auth.c ../../kern/audit.c
	$(CC) $(CFLAGS) -I../../include $^ -o $@

clean:
		rm -f test_audit

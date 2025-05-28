CC ?= clang
# Additional compile flags from the top-level build system are appended.
CFLAGS += -std=gnu23 -Wall -Wextra -Werror
CPPFLAGS += -I../../include

all: test_audit


.PHONY: all clean

test_audit: test_audit.c ../../kern/auth.c ../../kern/audit.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_audit

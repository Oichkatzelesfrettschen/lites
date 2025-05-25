CC ?= gcc
# Additional compile flags from the top-level build system are appended.
CFLAGS += -std=c23 -Wall -Wextra -Werror

all: test_vm_fault

.PHONY: all clean

test_vm_fault: test_vm_fault.c ../../src-lites-1.1-2025/server/vm/vm_handlers.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_vm_fault

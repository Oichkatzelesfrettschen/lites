CC ?= gcc
CFLAGS ?= -std=c2x -Wall -Wextra

all: test_vm_fault

.PHONY: all clean

test_vm_fault: test_vm_fault.c ../../src-lites-1.1-2025/server/vm/vm_handlers.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	 rm -f test_vm_fault

ifeq ($(origin CC),default)
CC := $(shell command -v clang >/dev/null 2>&1 && echo clang || echo gcc)
endif
CFLAGS ?= -std=c23 -Wall -Werror

all: test_vm_fault

.PHONY: all clean

test_vm_fault: test_vm_fault.c ../../src-lites-1.1-2025/server/vm/vm_handlers.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	 rm -f test_vm_fault

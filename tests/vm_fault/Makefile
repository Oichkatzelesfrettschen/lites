CC ?= clang
# Additional compile flags from the top-level build system are appended.
CFLAGS += -std=gnu23 -Wall -Wextra -Werror -I$(SRC_DIR)/include

all: test_vm_fault

.PHONY: all clean

ifndef LITES_SRC_DIR
$(error LITES_SRC_DIR must be set)
endif
SRC_DIR := $(LITES_SRC_DIR)
VM_SRC := $(SRC_DIR)/libos/vm.c
ifneq ($(wildcard $(VM_SRC)),)
VM_PATH := $(VM_SRC)
else
VM_PATH := ../../libos/vm.c
endif

test_vm_fault: test_vm_fault.c $(VM_PATH) ../../posix.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_vm_fault

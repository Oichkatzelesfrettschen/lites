CC ?= clang
# Additional compile flags from the top-level build system are appended.
CFLAGS += -std=gnu23 -Wall -Wextra -Werror -I$(SRC_DIR)/include

all: test_vm_fault

.PHONY: all clean

ifndef LITES_SRC_DIR
$(error LITES_SRC_DIR must be set)
endif
SRC_DIR := $(LITES_SRC_DIR)
LIBOS_LIB := ../../libos/libos.a
ifneq ($(wildcard $(SRC_DIR)/libos/libos.a),)
LIBOS_LIB := $(SRC_DIR)/libos/libos.a
endif

test_vm_fault: test_vm_fault.c ../../posix.c $(LIBOS_LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_vm_fault

CC ?= clang
# Allow overriding the target architecture (e.g. ARCH=i386)
ARCH ?= x86_64
# Disable glibc fortify since we override libc functions
CFLAGS += -std=gnu23 -Wall -Wextra -Werror -U_FORTIFY_SOURCE
# Include the common and architecture specific headers from the modernised tree
ifndef LITES_SRC_DIR
$(error LITES_SRC_DIR must be set)
endif
SRC_DIR := $(LITES_SRC_DIR)
ifneq ($(wildcard $(SRC_DIR)/include),)
CPPFLAGS += -I$(SRC_DIR)/include -I$(SRC_DIR)/include/$(ARCH)
endif

all:
@echo "fifo test disabled"

prepare:
@:

.PHONY: all clean

#test_fifo: test_fifo.c $(LITES_SRC_DIR)/libposix/posix.c
#$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_fifo

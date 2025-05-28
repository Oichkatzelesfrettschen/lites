CC ?= clang
ARCH ?= x86_64
CFLAGS += -std=gnu23 -Wall -Wextra -Werror
ifndef LITES_SRC_DIR
$(error LITES_SRC_DIR must be set)
endif
SRC_DIR := $(LITES_SRC_DIR)
CPPFLAGS += -I../../include
ifneq ($(wildcard $(SRC_DIR)/include),)
CPPFLAGS += -I$(SRC_DIR)/include -I$(SRC_DIR)/include/$(ARCH)
endif

all:
@echo "posix test disabled"

prepare:
@:

.PHONY: all clean

#test_posix: test_posix.c $(LITES_SRC_DIR)/liblites/posix_wrap.c
#$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)


clean:
	rm -f test_posix

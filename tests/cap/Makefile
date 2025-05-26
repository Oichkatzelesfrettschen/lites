CC ?= clang
# Additional compile flags from the top-level build system are appended.
CFLAGS += -std=gnu23 -Wall -Wextra -Werror
CPPFLAGS += -I../../include
SRC_DIR ?= ../../src-lites-1.1-2025
ifneq ($(wildcard $(SRC_DIR)/include),)
CPPFLAGS += -I$(SRC_DIR)/include
endif
CAP_SRC := $(SRC_DIR)/server/kern/cap.c
ifneq ($(wildcard $(CAP_SRC)),)
CAP_PATH := $(CAP_SRC)
else
CAP_PATH := ../../server/kern/cap.c
endif

all: test_cap


.PHONY: all clean

test_cap: test_cap.c $(CAP_PATH) ../../kern/auth.c ../../kern/audit.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_cap

CC ?= clang
ARCH ?= x86_64
CFLAGS += -std=gnu23 -Wall -Wextra -Werror
CPPFLAGS += -I../../src-lites-1.1-2025/include \
           -I../../src-lites-1.1-2025/include/$(ARCH)

all:
@echo "posix test disabled"

prepare:
@:

.PHONY: all clean

#test_posix: test_posix.c ../../src-lites-1.1-2025/liblites/posix_wrap.c
#$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)


clean:
	rm -f test_posix

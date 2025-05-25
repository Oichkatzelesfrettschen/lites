CC ?= clang
CFLAGS += -std=c2x -Wall -Wextra -Werror
CPPFLAGS += -I../../include

all: dirlist

.PHONY: all clean

dirlist: dirlist.c ../../src-lites-1.1-2025/liblites/memfs.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f dirlist

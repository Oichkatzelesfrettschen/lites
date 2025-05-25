CC ?= gcc
CFLAGS += -std=c23 -Wall -Wextra -Werror
CPPFLAGS += -I../../src-lites-1.1-2025/include

all: test_signal

.PHONY: all clean

test_signal: test_signal.c ../../src-lites-1.1-2025/liblites/signal_helpers.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_signal

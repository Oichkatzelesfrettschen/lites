CC ?= gcc
CFLAGS ?= -std=c23 -O2
LDFLAGS ?=

all: user_pager

user_pager: main.c
$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
rm -f user_pager

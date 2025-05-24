CC ?= gcc
CFLAGS ?= -std=c2x -O2
LDFLAGS ?=

all: user_pager

user_pager: main.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	 rm -f user_pager

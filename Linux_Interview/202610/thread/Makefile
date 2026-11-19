CC      ?= gcc
CFLAGS  ?= -Wall -Wextra -g -O0
LDFLAGS ?= -pthread

SRCS := $(wildcard *.c)
BINS := $(SRCS:.c=)

.PHONY: all run clean
all: $(BINS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

run: all
	@for b in $(BINS); do \
		echo "==================== $$b ===================="; \
		./$$b; \
	done

clean:
	rm -f $(BINS)

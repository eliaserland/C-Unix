SRCS = $(wildcard *.c)
#SRC_DIRS := .
#SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')

PROGS = $(patsubst %.c, %, $(SRCS))

CC = gcc
CFLAGS = -g -std=gnu11 -Wall -Wextra -Wpedantic -Wmissing-declarations -Wmissing-prototypes -Wold-style-definition -Werror -pthread

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(H)
clean: 
	rm -f $(PROGS)

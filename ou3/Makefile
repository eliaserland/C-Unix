# Makefile for OU3, C Programming and Unix - Umea University Autumn 2021
# Author: Elias Olofsson (tfy17eon@cs.umu.se)
# Version information:
#   2021-10-15: v1.0, first public version. 

CC = gcc
CFLAGS = -g -std=gnu11  -Wall -Wextra -Wpedantic -Wmissing-declarations  -Wold-style-definition -Wmissing-prototypes #-Werror
LDFLAGS = -pthread

TARGET = mdu

SRC = $(TARGET:=.c) list.c
OBJ = $(SRC:.c=.o)

all: mdu

mdu: mdu.o list.o list.h
	$(CC) $(LDFLAGS) -o $@ $^

mdu.o: mdu.c
	$(CC) $(CFLAGS) -c $^

list.o: list.c
	$(CC) $(CFLAGS) -c $^ 
	
memtest: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes --track-fds=yes ./$< /etc -j8

helgrind: $(TARGET)
	valgrind --tool=helgrind ./$< /etc -j8
clean: 
	-rm -f $(TARGET) $(OBJ) stdout.txt stderr.txt 

# Makefile for OU2, C Programming and Unix - Umea University Autumn 2021
# Author: Elias Olofsson (tfy17eon@cs.umu.se)
# Version information:
#   2021-10-01: v1.0, first public version. 

CC = gcc
CFLAGS = -g -std=gnu11  -Wall -Wextra -Wpedantic -Wmissing-declarations -Wmissing-prototypes -Wold-style-definition #-Werror
#LDFLAGS = 

TARGET = mmake

SRC = $(TARGET:=.c) parser.c parser.h
OBJ = $(TARGET:=.o) parser.o

all: mmake

mmake: mmake.o parser.o
	$(CC) $(LDFLAGS) -o $@ $^

mmake.o: mmake.c parser.h
	$(CC) $(CFLAGS) -c $*.c 

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c $*.c 
	
clean: 
	-rm -f $(TARGET) $(OBJ)
	
memtest: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes --track-fds=yes ./$<

# Makefile for OU1, C Programming and Unix - Umea University Autumn 2021
# Author: Elias Olofsson (tfy17eon@cs.umu.se)
# Version information:
#   2021-09-21: v1.0, first public version.
#   2021-10-10: v2.0, updated to reflect dependecies of the supplementary data
#                     structure in list.c and list.h.
 
CC = gcc
CFLAGS = -g -std=gnu11 -Wall -Wextra -Wpedantic -Wmissing-declarations -Wmissing-prototypes -Wold-style-definition -Werror
#LDFLAGS = 

TARGET = mexec

SRC = $(TARGET:=.c) list.c
OBJ = $(SRC:.c=.o)

all: mexec

mexec: mexec.o list.o list.h
	$(CC) -o $@ $^ $(LDFLAGS)

mexec.o: mexec.c
	$(CC) -c $^ $(CFLAGS)

list.o: list.c 
	$(CC) -c $^ $(CFLAGS)
clean: 
	-rm -f $(TARGET) $(OBJ)
	
memtest: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes --track-fds=yes ./$<

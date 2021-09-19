# Makefile for OU1, C Programming and Unix - Umea University Autumn 2021
# Author: Elias Olofsson (tfy17eon@cs.umu.se)
# Version information:
#   2021-09-21: v1.0, first public version. 

CC = gcc
CFLAGS = -g -std=gnu11 -Werror -Wall -Wextra -Wpedantic -Wmissing-declarations -Wmissing-prototypes -Wold-style-definition
#LDFLAGS = 

TARGET = mexec

SRC = $(TARGET:=.c)
OBJ = $(TARGET:=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS) 

$(OBJ): $(SRC)
	$(CC) -c $^ $(CFLAGS)
	
clean: 
	-rm -f $(TARGET) $(OBJ)
	
memtest: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes --track-fds=yes ./$<

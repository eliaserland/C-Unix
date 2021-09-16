# Makefile for OU1, C Programming and Unix - Umea University Autumn 2021
# Author: Elias Olofsson (tfy17eon@cs.umu.se)
# Version information:
#   2021-09-21: v1.0, first public version. 

CC = gcc
CFLAGS = -g -std=gnu11 -Werror -Wall -Wextra -Wpedantic -Wmissing-declarations -Wmissing-prototypes -Wold-style-definition
#CFLAGS = -g -std=gnu11 -Wall
#LDFLAGS = 

all: mexec

mexec: mexec.o  
	$(CC) -o $@ $^ $(LDFLAGS)  

mexec.o: mexec.c
	$(CC) -c $^ $(CFLAGS)
	
clean: 
	-rm -f mexec mexec.o
	
memtest: mexec
	valgrind --leak-check=full --show-reachable=yes ./$<

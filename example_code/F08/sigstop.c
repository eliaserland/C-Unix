#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void sighand ( int );

int main ( int argc, char *argv[] ) {
  if (argc != 1) {
    fprintf(stderr, "No arguments are allowed!\n");
    return 1;
  }
  if (signal(SIGUSR1, sighand) == SIG_ERR) {
    fprintf(stderr, "Oops, couldn't register SIGUSR1\n");
    perror(argv[0]);
        exit(1);
  }
  if (signal(SIGSTOP, sighand) == SIG_ERR) {
    fprintf(stderr, "Oops, couldn't register SIGSTOP\n");
    perror(argv[0]);
    exit(1);
  }
  return 0;
}

void sighand ( int x ) {
  fprintf(stderr, "Hi there %d \n", x);
}

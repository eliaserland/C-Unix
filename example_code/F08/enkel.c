#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static void sigCatcherUSR1( int );

int main ( int argc, char *argv[] ) {
  if (argc != 1) {
    fprintf(stderr, "No arguments allowed\n");
    return 1;
  }
  if (signal(SIGUSR1, sigCatcherUSR1) == SIG_ERR) {
    fprintf(stderr, "Couldn't register signal handler\n");
    perror(argv[0]);
    exit(1);
  }
    
  for (;;) {
    sleep(1);
  }

  return 0;
}

static void sigCatcherUSR1( int theSignal ) {
  if (theSignal == SIGUSR1) {
    fprintf(stdout, "I got the signal\n");
  } else {
    fprintf(stdout, "Wrong signal\n");
  }
}

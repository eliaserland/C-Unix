#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sighand ( int );

int main ( void ) {
  int i = 4;
  signal(SIGUSR1, sighand);
  signal(SIGABRT, sighand);
  signal(SIGUSR2, sighand);
  signal(SIGINT, sighand);
    
  while (i--) {
    sleep(5);
    fprintf(stderr, "Tittar mig omkring\n");
  }
  fprintf(stderr, "Före abort\n");
  abort();
  fprintf(stderr, "Efter abort\n");

  return 0;
}

void sighand ( int x ) {
  if ( x == SIGUSR1) {
    fprintf(stderr, "En SIGUSR1 signal\n");
  } else if ( x == SIGABRT ) {
    fprintf(stderr, "En SIGABRT signal\n");
  } else if ( x == SIGUSR2) {
    fprintf(stderr, "En SIGUSR2 signal\n");
  } else if ( x == SIGINT) {
    fprintf(stderr, "En SIGINT signal\n");
  } else {
    fprintf(stderr, "Okänt fel\n");
  }
  fprintf(stderr, "Lämnar avbrottshanteraren\n");
  return;
}


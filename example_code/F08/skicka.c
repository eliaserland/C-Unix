#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void childSigHandler( int x );
void doChildStuff( void );

int main ( void ) {
  int pid = fork();
    
  if ( pid < 0 ) {
    perror("main");
    exit(1);
  }
    
  if ( pid == 0 ) {
    fprintf(stdout, "Child says hello\n");
    doChildStuff();
    fprintf(stdout, "Child says goodbye\n");
  } else {
    fprintf(stdout, "Parent says hello\n");
    sleep(10);
    fprintf(stdout, "Wake up!\n");
    if (kill(pid, SIGUSR1)) {
      fprintf(stderr, "Couldn't send signal\n");
    }
    fprintf(stdout, "Parent waiting\n");
    waitpid(pid, NULL, 0);
    fprintf(stdout, "Parent says goodbye\n");
  }
  exit(0);
}
void childSigHandler( int x ) {
  if (x == SIGUSR1)
    fprintf(stdout, "Child says: No dad, it's to early to get up\n");
}

void doChildStuff( void ) {
  int i = 10;
    
  signal(SIGUSR1, childSigHandler);
  while (i--) {
    sleep(2);
    fprintf(stdout, "Child snores\n");
  }
  fprintf(stdout, "Child wakes up\n");
}

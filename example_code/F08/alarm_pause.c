#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void childSigHandler( int x );
void parentSigHandler( int x );
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
    signal(SIGALRM, parentSigHandler);
    alarm(6);
    fprintf(stdout, "Parent goes to sleep\n");
    pause();
    fprintf(stdout, "Parent says: Wake up kid, time to go to school\n");
        
    if (kill(pid, SIGUSR1)) {
      fprintf(stderr, "Couldn't send signal\n");
    }
    fprintf(stdout, "Parent says goodbye\n");
  }

  return 0;
}
void childSigHandler( int x ) {
  if (x == SIGUSR1)
    fprintf(stdout, "Child says: No dad, it's to early to get up\n");
}

void parentSigHandler( int x ) {
  if (x == SIGUSR1)
    fprintf(stdout, "Parent says: time to wake the kid\n");
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

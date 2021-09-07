#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

int main ( void ){
    
  char *exampleArgs[] = {"Funky","business", "at hand", NULL};
  pid_t pid;
    
  /* Börja med att skapa en process direkt */
  pid = fork();
    
  /* Kolla vad som hände */
  if ( pid < 0 ) {
    perror("fork:");
    exit(1);
  } else if ( pid == 0 ) {
    sleep(5);
    /* Barnprocessen: starta testprogrammet */
    if ( execv("sillyprog", exampleArgs) < 0 ) {
      perror("Exec:");
      exit(1);
    }
    printf("This line is never executed\n");
  } else {
    /* Föräldraprocessen */
    sleep(3);
    printf("Parent signing off \n");
  }
        
  return 0;
}

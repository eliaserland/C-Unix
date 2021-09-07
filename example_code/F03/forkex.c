#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int g = 10;
int main(void) { 
  int v = 10;
  pid_t p;
  if ((p = fork()) < 0) {
      perror("fork error");
      return 1;
  } else if (p == 0) {
      v++;
      g++;
  } else {
      g--;
      v--;
      if (waitpid(p, NULL, 0) != p) {
          perror("waitpid error");
          return 1;

      }
  }
  printf("mypid = %d parentpid = %d p = %d v = %d g = %d\n",
         getpid(), getppid(), p, v, g);
  return 0;
}


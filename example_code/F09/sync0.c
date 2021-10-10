#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NTHR  4
#define FALSE 0
#define TRUE  1

void *worker(void *arg);

int NotDone=TRUE;

int main(void)
{
  pthread_t trd[NTHR];
  int i;

  for (i = 0; i < NTHR; i++)
    pthread_create(&trd[i], NULL, worker, NULL);

  for (i = 0; i < NTHR; i++)
    pthread_join(trd[i], NULL);

  printf("All done\n");
  return 0;
}

void *worker(void *arg)
{
  if (arg != NULL) {
     write(STDERR_FILENO, "worker: wrong argument\n", 26);
     return (void *)1;
  }
  if (NotDone) {
     write(STDIN_FILENO, "Doing something just once\n", 26);
     NotDone = FALSE;
  }

  write(STDIN_FILENO, "Doing something\n", 16);

  return 0;
}

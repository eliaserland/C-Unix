#include <pthread.h>
#include <stdio.h>

#define NELEM 4096
#define NTHR  16

void *worker(void *arg);

int arr[NELEM], sum=0;

int main(void)
{
  pthread_t trd[NTHR];
  int i, startindex;

  for (i = 0; i < NELEM; i++)
    arr[i] = i;

  for (i = 0; i < NTHR; i++) {
    startindex = i * NELEM / NTHR;
    pthread_create(&trd[i], NULL, worker,
      (void *)&startindex);
  }

  for (i = 0; i < NTHR; i++)
    pthread_join(trd[i], NULL);

  printf("Sum is %d\n", sum);
  return 0;
}

void *worker(void *arg)
{
  int i;
  int base_idx;

  base_idx = *(int *)arg;
  for (i = base_idx; i < base_idx + NELEM / NTHR; i++)
    sum += arr[i];

  return NULL;
}

#include <pthread.h>
#include <stdio.h>

#define NELEM 8192
#define NTHR  8

void *worker(void *arg);

int arr[NELEM], sum=0;
pthread_mutex_t mtx_sum;

int main(void)
{
  pthread_t trd[NTHR];
  int i, startindex[NTHR];

  for (i = 0; i < NELEM; i++)
    arr[i] = i;

  pthread_mutex_init(&mtx_sum, NULL);

  for (i = 0; i < NTHR; i++) {
    startindex[i] = i * NELEM / NTHR;
    pthread_create(&trd[i], NULL, worker,
      (void *)&startindex[i]);
  }

  for (i = 0; i < NTHR; i++)
    pthread_join(trd[i], NULL);

  printf("Sum is %d\n", sum);
  return 0;
}

void *worker(void *arg)
{
  int i, lsum=0;
  int base_idx;

  base_idx = *(int *)arg;
  for (i = base_idx; i < base_idx + NELEM / NTHR; i++)
    lsum += arr[i];

  pthread_mutex_lock(&mtx_sum);
  sum += lsum;
  pthread_mutex_unlock(&mtx_sum);

  return NULL;
}

#include <stdio.h>

void worker(int base_idx);

#define NELEM 4096

int arr[NELEM], sum=0;

int main(void)
{
  int i, startindex;

  for (i = 0; i < NELEM; i++)
    arr[i] = i;

  startindex = 0;
  worker(startindex);
  startindex = NELEM / 2;
  worker(startindex);

  printf("Sum is %d\n", sum);
  return 0;
}

void worker(int base_idx)
{
  int i;

  for (i = base_idx; i < base_idx + NELEM / 2; i++)
    sum += arr[i];

  return;
}

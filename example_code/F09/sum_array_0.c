#include <stdio.h>
#define NELEM 4096

void worker(int base_idx);

int arr[NELEM], sum=0;

int main(void)
{
  int i, startindex;
  for (i = 0; i < NELEM; i++)
    arr[i] = i;
  startindex = 0;
  worker(startindex);
  printf("Sum is %d\n", sum);
  return 0;
}

void worker(int base_idx)
{
  int i;

  for (i = base_idx; i < NELEM; i++)
    sum += arr[i];
  return;
}

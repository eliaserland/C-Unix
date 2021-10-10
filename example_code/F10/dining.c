
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

void* wait_for_forks(void *param);

char *philosofers[5]={"Descartes","Platon","Sokrates","Aristoteles","Hegel"};
pthread_mutex_t forks[5];

int main(void)
{
    unsigned long int i;
    void *status;
    pthread_t trd[5];
    for (i = 0; i < 5; i++)
        pthread_mutex_init(forks+i, NULL);
    for (i = 0; i < 5; i++) {
        pthread_create(trd+i, NULL, wait_for_forks, (void *) i);
    }
    for (i = 0; i < 5; i++)
        pthread_join(trd[i],&status); /*wait for all threads*/
    return 0;
}

void* wait_for_forks(void *param)
{

   unsigned long int philosofer=(unsigned long int) param;
   struct timespec spec2 = {0, 0};
   struct timespec spec = {0, 0};
   int vgaffel = philosofer;
   int hgaffel = (philosofer+1)%5;
   int j;

   /*
   if (philosofer == 4) {
      int temp = vgaffel;
      vgaffel = hgaffel;
      hgaffel = temp;
   }
   */
   for (j = 0; j < 20; j++)
   {
       /*
       spec2.tv_sec = rand()%4;
       spec.tv_sec = rand()%4;
       */
       spec2.tv_sec = 0;
       spec.tv_sec = 0;

       pthread_mutex_lock(&forks[vgaffel]);
         printf("%s plockade upp gaffel %d\n", philosofers[philosofer], vgaffel);
       pthread_mutex_lock(&forks[hgaffel]);
         printf("%s plockade upp gaffel %d\n", philosofers[philosofer], hgaffel);

         printf("%s äter\n", philosofers[philosofer]);
         nanosleep(&spec2, NULL);
       pthread_mutex_unlock(&forks[hgaffel]);
       pthread_mutex_unlock(&forks[vgaffel]);

       printf("%s la tillbaka gafflarna\n", philosofers[philosofer]);
       printf("%s tänker igen\n", philosofers[philosofer]);
       nanosleep(&spec, NULL);
   }
   return NULL;
}

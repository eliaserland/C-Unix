#define _POSIX_C_SOURCE 200112L

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define    BUF_SIZE    100

void *readerCode ( void *dummy);
void *writerCode ( void *dummy);

int buffer[BUF_SIZE];
int first_free = 0;
int oldest_item = 0;
int buffer_full = 0;
int buffer_empty = 1;

pthread_mutex_t bufferGuard;

struct timespec shortdelay={0,500000000};

struct timespec delay={1,0}; /*1 sekund 0 nanosekunder*/

int main ( void )
{
    pthread_t   reader;
    pthread_t   writer;
    pthread_t   writer2;

    pthread_mutex_init(&bufferGuard,NULL);    
    if (pthread_create(&reader, NULL, &readerCode, NULL)) {
        fprintf(stderr, "Couldn't create reader thread\n");
        exit(1);
    }

    if (pthread_create(&writer, NULL, &writerCode, NULL)) {
        fprintf(stderr, "Couldn't create writer thread\n");
        exit(1);
    }

    if (pthread_create(&writer2, NULL, &writerCode, NULL)) {
        fprintf(stderr,"Couldn't create writer thread\n");
        exit(1);
    }
   
    pthread_join(reader,NULL);
    pthread_join(writer,NULL);
    pthread_join(writer2,NULL);
	return 0;
}

void *readerCode ( void *dummy)
{
    int consume;
    int b;

    if (dummy != NULL) {
        write(STDERR_FILENO, "worker: wrong argument\n", 26);
        return (void *)1;
    }
    
    while (1) {
        printf("Reader : trying to get into critical region\n");
        if (pthread_mutex_lock(&bufferGuard)) {
            fprintf(stderr, "Fel vid mutexlas");
            exit(1);
        }
        printf("Reader : inside critical region\n");
        if ((b = !buffer_empty)) {
            consume         = buffer[oldest_item];
            buffer_full     = 0;
            oldest_item     = (oldest_item + 1) % BUF_SIZE;
            buffer_empty    = first_free == oldest_item;
        }
        printf("Reader : trying to leave critical region\n");
        pthread_mutex_unlock(&bufferGuard);
        if (b) /*gor utskriften efter att ha slappt laset for att
                 halla det sa kort tid som mojligt*/
           printf("Reader consumed: %d\n", consume);
        else
           printf("Reader NOTHING consumed: %d\n", consume);
        sched_yield();
        nanosleep(&shortdelay, NULL);
    }
	return NULL;
}

void *writerCode ( void *dummy)
{
    int produce = 0;

    if (dummy != NULL) {
        write(STDERR_FILENO, "worker: wrong argument\n", 26);
        return (void *)1;
    }
    
    while (1) {
        printf("Writer : trying to get into critical region\n");
        pthread_mutex_lock(&bufferGuard);
        printf("Writer : inside critical region\n");
        if (!buffer_full) {
            buffer[first_free]  = produce;
            buffer_empty        = 0;
            first_free          = (first_free + 1) % BUF_SIZE;
            buffer_full         = first_free == oldest_item;
            printf("Writer produced: %d\n", produce);
        } else {
            printf("Writer NOTHING produced: %d\n", produce);
        }
        printf("Writer : trying to leave critical region\n");
        pthread_mutex_unlock(&bufferGuard);
        sched_yield();
        nanosleep(&delay, NULL);
        produce = (produce + 1) % 20;
    }
	return NULL;
}

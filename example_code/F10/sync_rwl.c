#define _POSIX_C_SOURCE 200112L


#include <pthread.h>
#include <stdio.h>
#include <time.h>

#define NELEM 4096
#define NTHR  4
#define ITER  10

void *reader(void *arg);
void *writer(void *arg);

pthread_rwlock_t rwl;

int main(void)
{
	pthread_t trd[NTHR], write_thread;
	unsigned long int i;

	pthread_rwlock_init(&rwl, NULL);

	for (i = 0; i < NTHR; i++)
		pthread_create(&trd[i], NULL, reader, (void *)i);

	pthread_create(&write_thread, NULL, writer, (void *)0);

	for (i = 0; i < NTHR; i++)
		pthread_join(trd[i], NULL);
	pthread_join(write_thread, NULL);

	return 0;
}

void *reader(void *arg) {
	int i;
	unsigned long int id = (unsigned long int)arg;
	struct timespec sleeptime = {1, 90000000};

	for (i = 0; i < ITER; i++) {
		pthread_rwlock_rdlock(&rwl);
		printf("Reader %ld reading\n", id);
		nanosleep(&sleeptime, NULL); /* simulate doing something */
		pthread_rwlock_unlock(&rwl);
		nanosleep(&sleeptime, NULL); /* simulate doing something else*/
	}
	return NULL;
}

void *writer(void *arg) {
	int i;
	unsigned long int id = (unsigned long int)arg;
	struct timespec sleeptime = {1, 90000000};

	for (i = 0; i < ITER; i++) {
		pthread_rwlock_wrlock(&rwl);
		printf("Writer %ld writing\n", id);
		nanosleep(&sleeptime, NULL); /* simulate doing something */
		pthread_rwlock_unlock(&rwl);
		nanosleep(&sleeptime, NULL); /* simulate doing something else*/
	}
	return NULL;
}

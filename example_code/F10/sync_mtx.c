#define _POSIX_C_SOURCE 200112L

#include <pthread.h>
#include <stdio.h>
#include <time.h>

#define NTHR  4
#define ITER  10

void *reader(void *arg);
void *writer(void *arg);

pthread_mutex_t mtx;

int main(void)
{
	pthread_t trd[NTHR], write_thread;
	unsigned long int i;

	pthread_mutex_init(&mtx, NULL);

	for (i = 0; i < NTHR; i++)
		pthread_create(&trd[i], NULL, reader, (void *)i);

	pthread_create(&write_thread, NULL, writer, (void *)0);

	for (i = 0; i < NTHR; i++)
		pthread_join(trd[i], NULL);
	pthread_join(write_thread, NULL);

	return 0;
}

void *reader(void *arg) {
	unsigned long int i, id = (unsigned long int)arg;
	struct timespec sleeptime = {1, 90000000};

	for (i = 0; i < ITER; i++) {
		pthread_mutex_lock(&mtx);
		printf("Reader %ld reading\n", id);
		nanosleep(&sleeptime, NULL); /* simulate doing something */
		pthread_mutex_unlock(&mtx);
		nanosleep(&sleeptime, NULL); /* simulate doing something else*/
	}
	return NULL;
}

void *writer(void *arg) {
	unsigned long int i, id = (unsigned long int)arg;
	struct timespec sleeptime = {1, 90000000};

	for (i = 0; i < ITER; i++) {
		pthread_mutex_lock(&mtx);
		printf("Writer %ld writing\n", id);
		nanosleep(&sleeptime, NULL); /* simulate doing something */
		pthread_mutex_unlock(&mtx);
		nanosleep(&sleeptime, NULL); /* simulate doing something else*/
	}
	return NULL;
}

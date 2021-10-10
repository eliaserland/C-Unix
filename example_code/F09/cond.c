#define _POSIX_C_SOURCE 200112L


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* condwaiter(void *arg);

int x, y;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int main(void) {
	pthread_t condThread;
	y = 10;
	x = 0;
	pthread_create(&condThread, NULL, &condwaiter, NULL);
	pthread_create(&condThread, NULL, &condwaiter, NULL);
	while (x < 20) {
		struct timespec sleeptime = {0, 50000000};
		pthread_mutex_lock(&mut);
		x++;
		if (x > y)
			pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mut);
		printf("main: x = %d y = %d\n", x, y);
		nanosleep(&sleeptime, NULL); /*simulate doing something*/
	}
	printf("Main Ready!\n");
	return 0;
}

void* condwaiter(void *arg) {
	/* Waiting until y is greater than x is performed as follows:*/
	struct timespec longsleeptime = {4, 0};

    if (arg != NULL) {
       write(STDERR_FILENO, "worker: wrong argument\n", 26);
       return (void *)1;
    }

	pthread_mutex_lock(&mut);
	while (x <= y) {
		printf("Waiting for x = %d y = %d\n", x, y);
		pthread_cond_wait(&cond, &mut);
	}
	/* operate on x and y */
	nanosleep(&longsleeptime, NULL); /*simulate doing something*/
x=8;
	printf("AT LAST! x = %d y = %d\n", x, y);
	pthread_mutex_unlock(&mut);
	printf("Thread finished!\n");
	return NULL;
}

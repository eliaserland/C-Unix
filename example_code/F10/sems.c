
#include    <stdio.h>
#include    <stdlib.h>
#include    <sys/types.h>
#include    <sys/ipc.h>
#include    <sys/sem.h>

int main(int argc, char *argv[])
{
    struct sembuf   ssignal = {0, 1, 0};
    int             semid;

    if (argc != 2 || sscanf(argv[1], "%d", &semid) != 1) {
        fprintf(stderr, "Usage: %s semid\n", argv[0]);
        exit(1);
    }

    printf("Innan signal\n");
    if (semop(semid, &ssignal, 1) == -1)
        perror("Error signaling for semaphore");
    else
        printf("Efter semaforen\n");
    exit(0);
}

#include    <stdio.h>
#include    <stdlib.h>
#include    <sys/types.h>
#include    <sys/ipc.h>
#include    <sys/sem.h>

int main(int argc, char *argv[])
{
    struct sembuf   swait = {0, -1, 0};
    int             semid, semkey = IPC_PRIVATE;

    if (argc != 1) {
        fprintf(stderr, "%s takes no argument\n", argv[0]);
        exit(1);
    }
    if ((semid = semget(semkey, 1, 0600 | IPC_CREAT | IPC_EXCL)) == -1) {
        perror("Can't create semaphore");
        exit(1);
    }

    printf("Före semaforen\n");
    if (semop(semid, &swait, 1) == -1)
        perror("Error waiting for semaphore");
    else
        printf("Efter semaforen\n");
    if (semctl(semid, 1, IPC_RMID) == -1)
        perror("Error removing semaphore");
    exit(0);
}

#include    <stdio.h>
#include    <stdlib.h>
#include    "semops.h"

int main(int argc, char *argv[])
{
    semaphore n;

    if (argc != 1) {
        fprintf(stderr,"%s takes no argument\n", argv[0]);
        exit(1);
    }
    if ((n = initsem(1001)) == -1) {
        perror("Can't create semaphore");
        exit(1);
    }

    if (killsem(n) == -1)
        perror("Error deleting the semaphore");
    printf("Deleted\n");
    exit(0);
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void child1(int parent2child[], int child2child[], int child2parent[]);
void child2(int parent2child[], int child2child[], int child2parent[]);

#define READ_END 0
#define WRITE_END  1

int main ( int argc, char *argv[] ) {
    int    parent2child[2];
    int    child2child[2];
    int    child2parent[2];
    pid_t  pid;
    int    number, ret;

    /* Test of number of arguments (should be 0) */
    if (argc != 1) {
        fprintf(stderr, "\n%s takes no argument\n\n", argv[0]);
        exit(1);
    }

    /* Open pipes  */
    if (pipe(parent2child) != 0 ) {
        perror("Pipe1");
        exit(1);
    }
    if (pipe(child2child) != 0 ) {
        perror("Pipe2");
        exit(1);
    }
    if (pipe(child2parent) != 0 ) {
        perror("Pipe3");
        exit(1);
    }
  
    /* Create a new process for child 1 */
    pid = fork();

    if (pid < 0) {
        perror("fork 1\n");
        exit(1);
    } else if (pid == 0) {      /* Child 1 */
        child1(parent2child, child2child, child2parent);
        exit(0);
    }

    /* Parent */
    /* Create a new process for child 2 */
    pid = fork();
  
    if (pid < 0) {
        perror("fork 2\n");
        exit(1);
    } else if (pid == 0) {      /* Child 2 */
        child2(parent2child, child2child, child2parent);
        exit(0);
    }


    /* Parent */
    int input = 0;

    /* Close non used pipes */
    close(parent2child[READ_END]);
    close(child2child[READ_END]);
    close(child2child[WRITE_END]);
    close(child2parent[WRITE_END]);

    /* Do the work */
    do {
        printf("Input an integer: ");
        while ((input = scanf("%d", &number)) != 1) {
            if (input == EOF)
                break;
            fprintf(stderr, "Conversion failed - must be an integer or EOF, try again\n");
            getchar();
        }
        if (input != EOF) {
            if (write(parent2child[WRITE_END], &number, sizeof(int)) == -1) {
                perror("write to child 1");
                exit(1);
            }
            ret = read(child2parent[READ_END], &number, sizeof(int));
            if (ret == -1) {
                perror("read from child 2");
                exit(1);
            }
            printf("The answer is = %d\n\n", number);
        }
    } while (input != EOF);

    /* Work finished close the rest of the pipes */
    close(parent2child[WRITE_END]);
    close(child2parent[READ_END]);

    /* Wait for both childs */
    for (int i = 0; i < 2; i++)
        if ((pid = wait(NULL)) == -1)
            perror("Error waiting");
        else
            printf("Child %d has terminated\n", pid);

    exit(0);
}

void child1(int parent2child[], int child2child[], int child2parent[])
{
	int ret, temp_nr;
    
    /* Close non used pipes */
    close(parent2child[WRITE_END]);
    close(child2child[READ_END]);
    close(child2parent[READ_END]);
    close(child2parent[WRITE_END]);

    while ((ret = read(parent2child[READ_END], &temp_nr, sizeof(int))) == sizeof(int)) {
        temp_nr++;
        if (write(child2child[WRITE_END], &temp_nr, sizeof(int)) == -1) {
            perror("write to child 2");
            exit(1);
        }
    }

    if (ret == -1) {
        perror("read from parent");
        exit(1);
    }

    /* Work finished close the rest of the pipes */
    close(parent2child[READ_END]);
    close(child2child[WRITE_END]);

    return;
}

void child2(int parent2child[], int child2child[], int child2parent[])
{
	int ret, local_nr;

    /* Close non used pipes */
    close(parent2child[READ_END]);
    close(parent2child[WRITE_END]);
    close(child2child[WRITE_END]);
    close(child2parent[READ_END]);

    while ((ret = read(child2child[READ_END], &local_nr, sizeof(int))) == sizeof(int)) {
        local_nr*=10;
        if (write(child2parent[WRITE_END], &local_nr, sizeof(int)) == -1) {
            perror("write to parent");
            exit(1);
        }
    }

    if (ret == -1) {
        perror("read from child 1");
        exit(1);
    }

    /* Work finished close the rest of the pipes */
    close(parent2child[READ_END]);
    close(child2child[WRITE_END]);
}


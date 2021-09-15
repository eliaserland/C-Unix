#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main (void) {
    
    char *exampleArgs[] = {"Funky", "business",
                         "at hand", NULL};
    pid_t pid;
    
    /* Börja med att skapa en process direkt */
    pid = fork();
    
    if (pid < 0) {
        perror("Fork:\n");
        exit(1);
    } else if (pid == 0) {
        sleep(5);
        /* Barnprocessen: starta testprogrammet */
        if (execvp("./sillyprog", exampleArgs) < 0) {
            perror("Exec:");
            exit(1);
        }
        printf("This line is never executed\n");
    } else {
        int status;
        /* Föräldraprocessen */
        wait(&status);
        printf("Parent signing off. Child exited with status %d \n", status);
        printf("WEXITSTATUS: %d\n", WEXITSTATUS(status));
        printf("WIFEXITED: %d\n"  , WIFEXITED(status));
        printf("WIFSIGNALED: %d\n", WIFSIGNALED(status));
        printf("WIFSTOPPED: %d\n" , WIFSTOPPED(status));
        return 0;
    }
    return 0;  /* Should never reach this line! */
}

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void randomSleep(void);

int main(void)
{
    pid_t processID;
    
    fprintf(stderr, "Bara en process (pid=%d)\n", getpid());
    
    processID = fork();
    
    switch (processID) {
        case -1:
            perror("Fork problem\n");
            exit(1);
        case 0:           /* Hit kommer barnet */
            fprintf(stderr, "Who is first?\n");
            while(1) {
                randomSleep();
                printf("Barnet\n");
            }
            break;
        default:           /* Här är föräldern */
            fprintf(stderr,
                "Barnets pid:%d  foraldern: %d\n",
                processID, getpid());
            while (1) {
                randomSleep();
                printf("Foraldern\n");
            }        
    }
    exit(0);
}

void randomSleep(void){
    sleep(rand()%4);
}

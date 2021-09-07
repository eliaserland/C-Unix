# include   <stdio.h>
# include   <unistd.h>
# include   <stdlib.h>

int main ( void )
{
    pid_t   pid1=-1, pid2=-1;
    
    printf("           %6s\t%6s\t%6s\t%6s\n", "PID", "PPID", "pid1", "pid2");

    printf("Utskrift 1 %6d\t%6d\t%6d\t%6d\n", getpid(), getppid(), pid1, pid2);

    pid1 = fork();

    pid2 = fork();

    printf("Utskrift 2 %6d\t%6d\t%6d\t%6d\n", getpid(), getppid(), pid1, pid2);
    
    exit(0);
}

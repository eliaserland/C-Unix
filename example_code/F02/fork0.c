# include   <stdio.h>
# include   <unistd.h>
# include   <stdlib.h>

int main ( void )
{
    printf("Utskrift 1\n");
fflush(stdout);
    fork();
    fork();
    printf("Utskrift 2\n");
    
    exit(0);
}

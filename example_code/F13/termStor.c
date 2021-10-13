#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int
main(void)
{
    struct termios  term;

    if (tcgetattr(STDOUT_FILENO, &term) < 0)
        fprintf(stderr, "tcgetattr error");

    printf("testing SOMETHING?\n");
    printf("OLCUC = %d\n", term.c_oflag & OLCUC);
    printf("%d",OLCUC);

    term.c_oflag |= OLCUC;

    if (tcsetattr(STDOUT_FILENO, TCSANOW, &term) < 0)
        fprintf(stderr, "tcsetattr error");

    printf("testing SOMETHING?\n");

    if (tcgetattr(STDOUT_FILENO, &term) < 0)
        fprintf(stderr, "tcgetattr error");
    printf("OLCUC = %d\n", term.c_oflag & OLCUC);

    exit(0);
}

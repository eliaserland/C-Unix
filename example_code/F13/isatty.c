#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
    printf("fd 0: %s\n", isatty(0) ? "tty" : "not a tty");
    if (isatty(0))
        printf("ttyname: %s\n", ttyname(0));

    printf("fd 1: %s\n", isatty(1) ? "tty" : "not a tty");
    if (isatty(1))
        printf("ttyname: %s\n", ttyname(1));

    printf("fd 2: %s\n", isatty(2) ? "tty" : "not a tty");
    if (isatty(2))
        printf("ttyname: %s\n", ttyname(2));

    exit(0);
}

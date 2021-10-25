#include <stdio.h>
#include <stdlib.h>

void print_int(int tal);

int main(void)
{
    int x, y;

    x = 10;
    y = ++x;
    printf("x = %d\ny = %d\n\n", x, y);

    x = 10;
    y = x++;
    printf("x = %d\ny = %d\n\n", x, y);

    x = 10;
    print_int(++x);

    x = 10;
    print_int(x++);

    x = 10;
    while (++x < 11)
        printf("LOOP 1: x = %d\n", x);
    print_int(x);


    x = 10;
    while (x++ < 11)
        printf("LOOP 2: x = %d\n", x);
    print_int(x);
    exit(0);
}

void print_int(int tal)
{
    printf("tal = %d\n", tal);
}

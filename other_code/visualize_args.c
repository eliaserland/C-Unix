#include <stdio.h>
#include <stdlib.h>

void visualize_args(int argc, char *argv[]);

int main (int argc, char *argv[]) {
    visualize_args(argc, argv);
    return 0;
}

void visualize_args(int argc, char *argv[]) { 
    printf("argc: %d", argc);
    for (int i = 0; i < argc; i++) {
        printf(", argv[%d]: %s", i, argv[i]);
    }
    printf("\n");
}

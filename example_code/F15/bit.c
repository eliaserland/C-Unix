# include <stdio.h>
#include "bit_print.h"


int main ( int argc, char *argv[] ){
	int i;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s heltal\n\n", argv[0]);
		return 1;
	}
	if (sscanf(argv[1], "%d", &i) != 1) {
		fprintf(stderr, "Usage: %s heltal\n\n", argv[0]);
		return 1;
	}

	printf("Talet %d lagras som:\n", i);
	bit_print(i);

	putchar('\n');

    return 0;
}


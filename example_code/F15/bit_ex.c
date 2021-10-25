# include <stdio.h>
#include "bit_print.h"


int main ( int argc, char *argv[] ){
	int n;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s heltal\n\n", argv[0]);
		return 1;
	}
	if (sscanf(argv[1], "%d", &n) != 1) {
		fprintf(stderr, "Usage: %s heltal\n\n", argv[0]);
		return 1;
	}

    bit_print(n);
	printf("\tn = %d\n", n);

    bit_print(0177);
	printf("\t0177\n\n");

    bit_print(n & 0177);
	printf("\tn & 0177\n\n");

    bit_print(n | 0177);
	printf("\tn | 0177\n\n");

    bit_print(n ^ 0177);
	printf("\tn ^ 0177\n\n");

    bit_print(~n);
	printf("\t~n\n\n");

    bit_print(n << 2);
	printf("\tn << 2\n\n");

    bit_print(n >> 2);
	printf("\tn >> 2\n\n");


    return 0;
}


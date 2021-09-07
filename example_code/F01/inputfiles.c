#include <stdio.h>
#include <stdlib.h>
#define BUFSIZE	1024

void readfile(FILE *);

int main(int argc, char *argv[])
{
	FILE *inputfile;

	if (argc == 1) {
		readfile(stdin);
	} else {
		for (int i = 1; i < argc; i++) {
			if ((inputfile = fopen(argv[i], "r")) == NULL) {
				fprintf(stderr, "Skipping ");
				perror(argv[i]);
				continue;
			} 
			readfile(inputfile);
			fclose(inputfile);
		}
	}
	exit(0);
}

void readfile(FILE *in)
{
	char buf[BUFSIZE];
	static int counter = 0;

	/* Read and write */
	printf("File number %d:\n", ++counter);
	while (fgets(buf, BUFSIZE, in) != NULL) {
		fputs(buf, stdout);
	}
	return;
}

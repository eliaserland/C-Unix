#include <stdio.h>
#include <stdlib.h>
#define BUFSIZE	1024

void readfile(FILE *);

int main(int argc, char *argv[])
{
	FILE *inputfile;

	if (argc == 1) {
		inputfile = stdin;
	} else if (argc == 2) {
		if ((inputfile = fopen(argv[1], "r")) == NULL) {
			perror(argv[1]);
			exit(1);
		}
	} else {
		fprintf(stderr, "Wrong number of arguments.\n");
		fprintf(stderr, "usage: %s file\n", argv[0]);
		/* OBS! ej: perror("eget felmeddelande"); */
		exit(1);
	}
	readfile(inputfile);
	fclose(inputfile);
	exit(0);
}

void readfile(FILE *in)
{
	char buf[BUFSIZE];

	/* Read and write */
	while (fgets(buf, BUFSIZE, in) != NULL) {
		fputs(buf, stdout);
	}
	return;
}

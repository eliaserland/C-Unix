#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#define MAXLINE 256

int main(int argc, char **argv)
{
  FILE *in = stdin;
  char line[MAXLINE];
  char c;

  if (argc > 2) {
    fprintf(stderr, "Usage %s [inputfile]\n", argv[0]);
    exit(EXIT_FAILURE);
  } else if (argc == 2) {
    if ((in = fopen(argv[1], "r")) == NULL) {
      perror(argv[1]);
      exit(EXIT_FAILURE);
    }
  }
 
  while (fgets(line, MAXLINE, in) != NULL) {
    if (sscanf(line, "%c", &c) != 1) {
      continue;
    } else if (c == '#') {
      continue;
    }
    system(line);
  }

  if (argc == 2)
    fclose(in);

  return 0;
}

#include <stdio.h>
#include <stdlib.h>

#define LSIZE 1024

int main ( int argc, char *argv[] ) {
  FILE *fp;
  char line[LSIZE];

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <FIFO>\n", argv[0]);
    return 1;
  }
  if ((fp = fopen(argv[1], "r+")) == NULL) {
    fprintf(stderr, "Error opening FIFO\n");
    perror(argv[1]);
    return 1;
  }
  while (1) {
    if (fgets(line, LSIZE, fp) == NULL)
      return 0;
    fputs(line, stdout);
  }
  fclose(fp);
  return 0;
}

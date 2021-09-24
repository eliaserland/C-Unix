#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>

#define MAXLINE 10

int main(int argc, char **argv)
{
  FILE *read_com, *write_com;
  char line[MAXLINE];

  if (argc != 3) {
    fprintf(stderr, "Usage %s <FIRST COMMAND> <SECOND COMMAND>\n", argv[0]);
    return -1;
  }
 
  if ((read_com = popen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Error: popen() on line %d\n", __LINE__);
    perror("");
    return -1;
  }
  
  if ((write_com = popen(argv[2], "w")) == NULL) {
    fprintf(stderr, "Error: popen() on line %d\n", __LINE__);
    perror("");
    pclose(read_com);
    return -1;      
  }
  
  /* Read the output from the command in argv[1] via
     the FILE pointer read_com line by line */

  while (fgets(line, MAXLINE, read_com) != NULL) {
    /* Send the recieved output as input to the command
       in argv[2] via the FILE pointer write_com */
    if (fputs(line, write_com) == EOF) {
      fprintf(stderr, "fputs()\n");
      return -1;
    }
  }

  pclose(read_com);
  pclose(write_com);

  return 0;
}

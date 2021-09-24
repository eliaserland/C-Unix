/* (ungefär Figure 4.3 i Stevens and Rago) */

#include    <stdio.h>
#include    <sys/types.h>
#include    <sys/stat.h>

int main(int argc, char *argv[])
{
  int         i;
  struct stat buf;
  char        *ptr;

  if (argc < 2) {
    fprintf(stderr, "%s: at least one filename as argument\n", argv[0]);
    return 1 ;
  }

  for (i = 1; i < argc; i++) {
    printf("%-10s: ", argv[i]);
    if (lstat(argv[i], &buf) < 0) {
      fprintf(stderr, "lstat error\n");
      perror(argv[i]);
      continue;
    }

    if      (S_ISREG(buf.st_mode))  ptr = "regular";
    else if (S_ISDIR(buf.st_mode))  ptr = "directory";
    else if (S_ISCHR(buf.st_mode))  ptr = "character special";
    else if (S_ISBLK(buf.st_mode))  ptr = "block special";
    else if (S_ISFIFO(buf.st_mode)) ptr = "fifo";
#ifdef  S_ISLNK
    else if (S_ISLNK(buf.st_mode))  ptr = "symbolic link";
#endif
#ifdef  S_ISSOCK
    else if (S_ISSOCK(buf.st_mode)) ptr = "socket";
#endif
    else                            ptr = "** unknown mode **";

    printf("%-17s:\t%ld\n", ptr, buf.st_size);
  }
  return 0;
}


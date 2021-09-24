#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#define MY_ERROR_CODE  1

int main ( int argc, char *argv[] ){
  struct stat theInfo;            /* all info om filen */

  if ( argc != 2 ) {
    /* Fel antal argument, skriv ut ett felmeddelande.  */
    fprintf(stderr,"Usage: %s filename\n", argv[0]);
    return MY_ERROR_CODE;
  }

  /* Prova nu att hamta ut information om filen */
    
  if (lstat(argv[1], &theInfo)) {
    /* Fel nar jag forsokta lasa info om filen */
    perror(argv[1]);
    return MY_ERROR_CODE;
  }
    
  /*
     Skriv ut informationen om filen. För exakt vilka falt som
     finns i 'struct stat' ge kommandot "man lstat" och läs den
     manualsida som finns dar.
  */
    
  printf("st_mode    = %d\n", theInfo.st_mode);
  printf("st_ino     = %ld\n", theInfo.st_ino);
  printf("st_dev     = %ld\n", theInfo.st_dev);
  printf("st_rdev    = %ld\n", theInfo.st_rdev);
  printf("st_nlink   = %ld\n", theInfo.st_nlink);
  printf("st_uid     = %d\n", theInfo.st_uid);
  printf("st_gid     = %d\n", theInfo.st_gid);
  printf("st_size    = %ld\n", theInfo.st_size);
  printf("st_atime   = %ld\n", theInfo.st_atime);
  printf("st_mtime   = %ld\n", theInfo.st_mtime);
  printf("st_ctime   = %ld\n", theInfo.st_ctime);
  printf("st_blksize = %ld\n", theInfo.st_blksize);
  printf("st_blocks  = %ld\n", theInfo.st_blocks);

  return 0;
}


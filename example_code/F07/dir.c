/*
  Detta program visar hur man tar reda pa information
  om filer med hjalp av kommandot stat()

  Det visar ocksa hur man listar filer i en given
  katalog samt hur man byter arbetskatalogen for programmet
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

int main(int argc, char **argv) {
  struct stat file_info, dir_info;
  DIR *curr_dir;
  struct dirent *dir_pointer;

  /* Make sure we use the program correctly */
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <directory>\n", argv[0]);
    return 1;
  }
  
  /* Check if <filename> really is a regular file */
  if (stat(argv[1], &file_info) < 0) {
    perror(argv[1]);
    fprintf(stderr, "Error: Can't stat file '%s' on line %d\n",
            argv[1], __LINE__); 
    return 1;
  }
  
  /* The macro S_ISREG() returns true if the given st_mode is a regular file */
  if (!S_ISREG(file_info.st_mode)) {
     fprintf(stderr, "'%s' is not a regular file\n", argv[1]);
     return 1;
  }


  /* Check i <directory> really is a directory */
  if (stat(argv[2], &dir_info) < 0) {
     perror(argv[2]);
     fprintf(stderr, "Error: Can't stat file '%s' on line %d\n",
             argv[2], __LINE__);
     return 1;
  }
  /* The macro S_ISDIR() returns true if the given st_mode is a directory */
  if (!S_ISDIR(dir_info.st_mode)) {
     fprintf(stderr, "'%s' is not a directory\n", argv[2]);
     return 1;
  }

  /* ===========================================
     Ok, both files are ok. Now print the times
     for the regular file and print all files in
     the directory
     =========================================== */
   
  /* Print the 3 time values of the regular file */
  printf("File: %s\n", argv[1]);
  printf("   st_atime   st_mtime   st_ctime\n");
  printf("   ==============================\n");
  printf("   %d %d %d\n\n", (int)file_info.st_atime,
          (int)file_info.st_mtime, (int)file_info.st_ctime);
  
  /* Open the directory */
  if ((curr_dir = opendir(argv[2])) == NULL) {
     perror(argv[2]);
     fprintf(stderr, "Error: Unable to open dir '%s' on line %d\n",
             argv[2], __LINE__);
     return 1;
  }
  else {
     printf("Directory '%s' is open\n", argv[2]);
  }
  
  /* Repeat for all files in the open dir */
  while ((dir_pointer = readdir(curr_dir)) != NULL){
     printf("%s\n", dir_pointer->d_name);
  }
  
  /* close the directory */
  closedir(curr_dir);
  fprintf(stdout, "Directory '%s' is closed\n\n", argv[2]);


  /* ===========================================
     Now change directory to the given directory
     and print the files again 
     =========================================== */
  /* Change current working directory
     It is only the current working directory for
     the program that is changed.
     When the program exits, you will still be in
     the directory you called the program from */

  if (chdir(argv[2]) < 0) {
     perror(argv[2]);
     fprintf(stderr, "Unable to go to directory '%s' on line %d\n",
             argv[2], __LINE__);
     return 1;
  }

  /* Open the directory "." = current directory */
  if ((curr_dir=opendir(".")) == NULL) {
     perror(".");
     fprintf(stderr, "Unable to open directory '%s' on line %d\n",
             argv[2], __LINE__);
     return 1;
  }
  else {
     printf("Directory '%s' is open\n", ".");
  }
  
  /* Repeat for all files in the opend directory */
  while ((dir_pointer = readdir(curr_dir)) != NULL) {
     printf("%s\n", dir_pointer->d_name);
  }

  /* Close the directory */
  closedir(curr_dir);
  printf("Directory '%s' is closed\n", ".");
  
  return 0;
}


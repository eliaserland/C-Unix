#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>

int print_pwdstruct(char *);
int print_all_pwdstruct(int);

/* This function takes a user name and prints the
   information in the user's password struct
   Returns 1 if success and -1 if not */

int print_pwdstruct(char *username) {
  struct passwd *pw_pointer;

  /* Get the password struct */
  if ((pw_pointer = getpwnam(username)) == NULL) {
      fprintf(stderr, "Error: Unable to getpwnam() for user '%s' on line %d\n",
              username, __LINE__);     
      return -1;
  }
  
  /* Print all fields in the password struct */
  fprintf(stdout, "User name          : %s\n", pw_pointer->pw_name);
  fprintf(stdout, "Encrypted password : %s\n", pw_pointer->pw_passwd);
  fprintf(stdout, "Numerical user ID  : %d\n", (int)pw_pointer->pw_uid);
  fprintf(stdout, "Numerical group ID : %d\n", (int)pw_pointer->pw_gid);
  fprintf(stdout, "Comment field      : %s\n", pw_pointer->pw_gecos);
  fprintf(stdout, "Initial work dir   : %s\n", pw_pointer->pw_dir);
  fprintf(stdout, "Initial shell      : %s\n", pw_pointer->pw_shell);

  return 0;
}


/* This function goes through all entries in the password
   file and uses print_pwdstruct() to print the information
   about all users in the system. When it is done it prints
   the number of users on the system 

   If print_check does not equal zero, then all user info is
   printed, else it is not
*/

int print_all_pwdstruct(int print_check) {
  struct passwd *ptr;
  int N=0;

  /* Reset the password file */
  setpwent();
  
  /* Repeat for all entrie in the pw file */
  while ((ptr = getpwent()) != NULL) {
      if (print_check) {
          print_pwdstruct(ptr->pw_name);
          fprintf(stdout, "\n");
      }
      
      N++;
  }

  /* Close the password file */
  endpwent();

  fprintf(stdout, "These are %d users on this system\n", N);

  return 0;
}




int main(int argc, char **argv)
{
  
  /* Make sure the program is used correctly */
  if (argc != 3) {
      fprintf(stderr, "Usage: %s <user name> <print check>\n", argv[0]);
      return -1;
  }

  
  print_pwdstruct(argv[1]);
  print_all_pwdstruct(atoi(argv[2]));

  return 0;
}


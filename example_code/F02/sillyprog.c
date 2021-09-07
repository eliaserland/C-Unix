# include   <stdio.h>

int main ( int argc, char *argv[] ){
  int i;
    
  for (i = 0; i < argc; i++) {
    fprintf(stdout, "Arg nr %d = %s\n", i, argv[i]);
  }
  return 7;
}

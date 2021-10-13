#include <unistd.h>
#include <stdio.h>

int main(void)
{
   char c='A';
   for (; c <= 'Z'; c++)
   {
      write(STDOUT_FILENO, &c, 1); 
      sleep(2);
   }
   return 0;
}

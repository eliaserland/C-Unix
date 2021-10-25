#include <stdio.h>
#include    <string.h>
#include    <stdlib.h>

/*
 * Kompilatorn optimerar minnesanvändandet och lägger variabler efter varandra
 * i minnet. Om man skriver in för långt användarnamn
 * kommer den att skriva över
 * till command strängen. I detta fall skulle
 * man kunna skriva in "XXXXXXXX/bin/sh;" så skulle man få igång ett shell
 * istället för att köra who|grep komandot
 *
 * Att tänka på: Läs inte in mer tecken än var bufferten klarar av.
 */

int main(void)
{
   char command[100]=" who | grep ";
   char uname[8];

   printf("Ange ditt användarnamn: ");

   scanf("%s",uname);
   strncat(command,uname,8);
   printf(command);
   system(command);
   putchar('\n');
   return 0;
}


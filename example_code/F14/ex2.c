#include <stdio.h>

/*
 * Kompilatorn optimerar minnesanvändandet och lägger variabler efter varandra
 * i minnet. Om man skriver in för långt förnamn kommer den att skriva över
 * till command-bufferten. Om command skulle vara ett kommando att köra (via
 * system) skulle man kunna skriva in "XXXXXXX rm -rf /" som fname varpå
 * commandskulle bli "rm -rf /" Kanske inte så bra kommando att köra... ;)
 *
 * Att tänka på: Läs inte in mer tecken än var bufferten klarar av.
 *
 * Kända buggar: Blaster (hade inte fullgod koll på vad som kom in via
 *                       nätverket utan skrev när i en för liten buffert.)
 *
 *               OpenSSH (Egentligen ett integer overflow som gjorde att man
 *                       allokerade en för liten buffert.)
 */

int main(void)
{
   char command[10] = "ls -l";
   char fname[8];

   printf("Ange ditt förnamn: ");
   fflush(stdout);
   gets(fname);
   printf("Förnamn:  %s\n", fname);
   printf("Kommando: %s\n", command);
   return 0;
}


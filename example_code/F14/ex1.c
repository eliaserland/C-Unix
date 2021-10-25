#include <stdio.h>
#include <stdlib.h>

/*
 * Om detta program använder argv[1] för att indexera i en
 * array kan man skicka in ett negativt tal för att kringgå
 * kontrollen. Om två arrayer ligger efter varandra i
 * minnet kan man då komma åt data i den andra arrayen fast
 * det kanske inte är tänkt att man ska kunna det.
 *
 * Att tänka på: Jämför inte unsigned och vanliga variabler.
 * I vissa fall (som det här) blir resultatet helt fel.
 *
 * Kända buggar:
 *    select-funktionen i OpenBSD-kärnan
 *    OpenSSH (liknande fel, men en integer overflow istället)
 *
 * Vidare läsning: http://www.phrack.org/show.php?p=60&a=6
 */

int main(int argc, char *argv[])
{
   unsigned int min_num = 10;
   if(argc < 2)
   {
      fprintf(stderr, "för få argument\n");
      exit(1);
   }
   if(atoi(argv[1]) < min_num)
   {
      fprintf(stderr, "För litet förstaargument\n");
      exit(2);
   }

   printf("YES! äntligen är argumentet >= min_num (%d >= %d)\n", atoi(argv[1]), min_num);
   return 0;
}


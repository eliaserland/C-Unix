#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>


/*
 * Programmet läser in ett lösenord utan att visa det för användaren och lagrar
 * det i minnet. Bara för att det lagras i minnet är det inte skyddat.
 *
 * Att tänka på: När man använt klart känslig data ska det minnesområdet
 * skrivas "sönder" Ett anrop till free räcker inte (om nu minnet är dynamisk
 * allokerat.) Loopa igenom strängen och sätt alla tecken till 'x' eller
 * liknande.
 *
 * För att testa koden:
 *
 * Se till att du kan få en core-fil. Görs genom att skriva limit core 4000 i
 * ditt shell (begränsar core-filen till 4MB). Starta sedan programmet och
 * skriv in namn och lösenord. Låt programmet ligga och köra (sover iofs, men
 * men). Starta en ny xterm / putty på samma dator. Hitta rätt på processen via
 * ps -ef | grep $USER När du hittat pid-nummret se till att få en
 * core-fil: kill -SEGV nummret-du-fick-via-ps
 *
 * Nu kan man gå igenom core-filen och kolla efter viktig information. Tex
 * strings är ett jättebra kommando som skriver ut alla "utskriftsvänliga"
 * strängar i en fil. string core | less
 *
 * Leta och hitta :)
 *
 * Kända buggar: Många shellskript lider av det här. En del är chmoddade så
 *               man enbart kan köra koden, inte läsa den. Men det räcker inte.
 *
 */

int main(void)
{
   char c;
   struct termios buf, savebuf;
   int i;
   char name[256];
   char passwd[256];


   printf("Username: ");
   fflush(stdout);
   fgets(name, 256, stdin);
   i = 0;

   tcgetattr(STDIN_FILENO, &savebuf);
   tcgetattr(STDIN_FILENO, &buf);
   buf.c_lflag &= ~(ICANON | ECHO);
   buf.c_cc[VMIN] = 1;
   buf.c_cc[VTIME] = 0;
   tcsetattr(STDIN_FILENO, TCSANOW, &buf);
   printf("Password: ");
   fflush(stdout);
   memset(passwd, 0, 256);

   while(i++ < 256)
   {
      if ((read(STDIN_FILENO, &c, 1)) != 1)
      {
         perror("Input error");
         break;
      }
      else if(c == 10 || c == 13)
         break;
      else
         passwd[i] = c;
   }
   tcsetattr (0, TCSAFLUSH, &savebuf);

   printf("\ndone\n");
   fflush(stdout);

   /* Nu fejkar vi att vi gör någonting vettigt i 10 min */
   sleep(600);

   exit(0);
}


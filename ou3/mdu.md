# Laboration 3 - `mdu`

I laborationen ska ni skriva ett C-program som beräknar hur stor plats en
uppsättning specificerade filer tar på disken. Om de specificerade filerna är
kataloger ska de traverseras rekursivt för att beräkna totala diskanvändningen
för katalogen. Om `-j` flaggan används ska katalogerna traverseras parallellt.

## Gränsyta

Programmet ska heta *mdu* och indata ska tas som argument när programmet körs.

### Beräkna diskanvändning för en eller flera fil eller katalog

```bash
mdu {fil} [filer ...]
```

Om användaren specificerar ett antal trådar, större än 1, ska detta antal
trådar användas för att göra sökningen parallellt. Observera att flaggan -j med
antal trådar ska kunna skrivas på godtyckligt ställe i argumentlistan. För att
läsa in denna korrekt ska ni använda er av `getopt`.

### Beräkna diskanvändning med flera trådar

```
mdu -j {antal trådar} {fil} [filer ...]
```

## Beteende

En viktig del av laborationen är att lista ut när trådarna ska avsluta. Fundera
på hur en tråd ska veta att allt arbete är utfört, det räcker inte med att
listan över kataloger att traversera är tom. En semafor eller en conditional
variable kommer att behövas för att lösa detta.

## Kompilering

Programmet ska gå att kompilera utan varningar med hjälp av `make` och den
`Makefile` ni levererar tillsammans med programmet. Separatkompilering, som
även tar i hänsyn till alla eventuella header-filer, ska användas. Observera
att er `Makefile` ska skrivas manuellt och ej genereras av något verktyg.

Vid kompilerig ska åtminstone följande flaggor användas för alla steg:

```
-g -std=gnu11 -Werror -Wall -Wextra -Wpedantic
-Wmissing-declarations -Wmissing-prototypes -Wold-style-definition
```

## Kontroll av anrop

Alla funktionsanrop som returnerar eller på annat sätt ger tillbaka en
indikation för hur anropet gick ska kontrolleras. Detta innefattar till exempel
`malloc` som returnerar `NULL` om minne ej gick att allokera.

Dessa funktioner behöver ni *inte* kontrollera:

- `write`, `printf` och `fprintf` (eller annan utskriftfunktion)
- `close` och `fclose`
- `free`
- `pthread_mutex_lock`, `pthread_mutex_unlock`, `pthread_mutex_destroy`
- `sem_post`, `sem_wait`

Om ett anrop misslyckats ska programmet skriva ut ett lämpligt felmeddelande på
*standard error* för att sedan avsluta med returkoden `EXIT_FAILURE`. Om
anropet ändrar värdet på `errno` vid fel ska `perror` användas för att skriva
ut felmeddelandet.

Det normala beteendet för UNIX program som av någon orsak inte kan läsa en
eller flera filer är att skriva ut ett felmeddelande och fortsätta med resten
av filerna/katalogerna. Ni skall därför inte avsluta programmet vid första
bästa problem utan hantera det snyggt, hoppa över problemområdet och fortsätta
med resten av katalogerna/filerna.

## Rapport

Till uppgiften ska du inte skriva en fullständig rapport, men det du skriver
ska vara skrivet på ett vetenskapligt sätt.  Rapporten ska innehålla följande
delar:

- Resonemang kring trådsäkerhet
- Analys över prestanda
- Diskussion och reflektion

I erat resonemang kring trådsäkerhet ska ni förklara hur ni gjort för att uppnå
trådsäkerhet i erat program och motivera varför det är trådsäkert.

Eran analys över prestanda ska innehålla en figur som visar vad som händer med
programmets körtid när antalet trådar ökar och ett resonemang kring varför ni
tror att programmets körtid ser ut som det gör.

På diskussion och reflektionsdelen kan du skriva om problem som du stötte på
när du gjorde laborationen, vad du tyckte om den och övriga åsikter som du vill
framföra.

## Analys och bash-skript
När ni analyserar prestandan av er lösning ska detta göras på Itchy med alla 
antal trådar från 1 till och med 100. För att göra detta ska ni skriva ett
bash-skript som gör körningarna åt er. Körningn ska kolla storleken på
katalogen */pkg/* på Itchy. Detta skript ska lämnas in på labres tillsammans med
er inlämning. Använd `time` for att ta tid på ert program, kolla gärna på hur ni
kan formatera utdatan som `time` ger ut. Det går att skriva ert bash skript så
att dess utdata går att läsa in direkt i ett program så som MATLAB.

## Övriga krav på lösningen

Utöver ovanstående nämnda beskrivning ska även programmet hålla sig till
kraven:

- Programmet ska ha samma output som `du -s -l -B1024 {fil} [filer ...]`
  förutom antalet blanka tecken mellan kolumner.
- Lösningen får *inte* använda `ftw`.
- Programmet ska använda sig av:
  - `lstat`
  - `opendir`
  - `readdir`
  - `closedir`
  - `pthread_create`
  - `pthread_join`
- Programmet ska använda sig av `getopt` samt `optind` för att läsa in
  argument.
- Programmet får ej ha några minnesläckor, använd `valgrind`.
- Programmet ska vara trådsäkert, använd `valgrind --tool=helgrind`
- Programmet ska ej busy waita
- Programmet ska kunna ta ett godtyckligt antal argument och kan därför ej
  använda hårdkodade storlekar på datatyper. Använd `realloc` eller en dynamisk
  datatyp för att lagra data.
- Koden ska skrivas med god kodkvalité (indentering, variabelnamn, kommentarer,
  funktionsuppdelning etc.).
- Inlämningen ska klara alla tester på Labres.
- Uppgiften ska lösas enskilt.

## Inlämning

Kod, makefile och rapport lämnas in via labres innan utsatt deadline.

## Tävling!

Vi har anordnat en liten tävling gällande labben för dem som vill!
Det är helt frivilling att vara med och det påverkar inte ert betyg negativt på något sätt ifall ni skippar. Så känner ni er stressade med uppgiften som det är så sluta läs nu.

Tävlingen kommer att gå ut på att vi mäter erat programs prestanda. Det som kommer mätas är wall-clock time precis som ni gör när ni analyserar er lösning för rapporten.
Hur vi gör denna mätning specifieras inte men vi kommer mäta ert program väldigt många gånger med ett specifikt antal trådar som är högre än vad maskinen har att ge.
Resultaten kommer sedan läggas upp i en rankad tabell/graf på kursen gitlab med ert namn och resultat.
De flesta av er har troligtvis aldrig optimiserat ett program förrut.

För att vara med i tävlingen ska ni lämna in en EXTRA makefile med namnet 'makefile\_competition'
Den ska kompilera ert tävlingsprogram som ska heta mdu\_competition.
Ni kan antingen bara återanvända er vanliga lösning i tävlingen eller skapa nya .c filer.
Skapar ni nya filer så bryr vi oss inte om kodkvaliten på dessa filer utan ni är fria att optimisera hur ni vill. Notera att detta gällar bara de filer som inte används till den orginella inlämningen.

Sätt ni kan testa att optimisera programmet på. Kom ihåg att mycket av detta är överkurs/onödigt så känn absolut inte att ni måste testa allt.
- Skriva en optimal algoritm gällande hur ni låser / öppnar trådar.
- Mäta funktioner hur lång tid specifika funktioner / delar av er kod tar så ni vet vart ni behöver förbättra koden. Funktionerna i \<time.h\> fungera ok här.
- Se till att en tråd alltid har något att göra.
- Ha programmets minne/cache i åtanke. https://en.wikipedia.org/wiki/Locality\_of\_reference
- Undvik att förvirra er processors branch predictor https://en.wikipedia.org/wiki/Branch\_predictor
- Använda er utav mer avancerade keywords som inline och register (är ni smartare än kompilatorn?)
- Ta bort all error checking (AHHHHHHHHHHHHHHHHHHHH). Går att göra på ett bra sätt via preprocessor defines dock.
- Undersök avancerade GCC flaggor. https://caiorss.github.io/C-Cpp-Notes/compiler-flags-options.html
- Testa googla 'how to optimize in c for newbies'

Lycka till!


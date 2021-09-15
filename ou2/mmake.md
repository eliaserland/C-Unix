# Laboration 2 - `mmake`

I laborationen ska ni implementera en minimal version av Unix-verktyget Make
i C.  Make är ett verktyg som används för att automatisera byggandet av
program.  Make läser in en Makefil som innehåller regler som avgör när olika
delar i programmet behöver byggas om.

Ett exempel på en Makefil är följande som kan användas för att bygga programmet
*mexec* från Laboration 1:

```make
mexec: mexec.o parser.o
        gcc -o mexec mexec.o parser.o

mexec.o: mexec.c
        gcc -c mexec.c

parser.o: parser.c
        gcc -c parser.c
```

Denna makefil används för att bygga en exekverbar fil vid namn *mexec*.  För
att kunna bygga *mexec* behöver den ha tillgång till objektfilerna *mexec.o*
och *parser.o*.  Objektfilerna *mexec.o* och *parser.o* har i sin tur en regler
för att kompileras utifrån motsvarande C-filer.

## Gränsyta

Programmet har följande synopsis:

```
mmake [-f MAKEFILE] [-B] [-s] [TARGET...]
```

Programmet ska stödja tre stycken frivilliga flaggor.  Flaggan `-f` används
för att använda en annan makefil än filen som heter *mmakefile* i nuvarande
katalog.  Flaggan `-B` används för att tvinga ombyggnation trots att filer
inte har uppdaterats.  Flaggan `-s` används för att inte skriva ut kommandon
till *standard output* när de exekveras.

Programmet kan också ta in en lista av targets som vanliga argument.  Dessa
argument avgör vilka targets som ska byggas.  Om inget target specificeras
kommer programmet bygga default target vilket beskrivs nedan.

## Filformat

Makefilen som programmet ska läsa in består av en eller flera regler som har
följande format:

```
target : [PREREQUISITE...]
        program [ARGUMENT...]
```

En regel består av två rader.  Första raden får inte börja med blanktecken och
innehåller target vilket är namnet på filen som regeln ska bygga.  Sedan kommer
ett kolon och en lista av prerequisites vilket är namn på filer som behövs
för att bygga target.  Prerequisites i listan separeras av blanktecken.
Listan av prerequisites avslutas vid nyrad.

Nästa rad börjar med ett tab tecken följt av ett kommando som ska exekveras
för att bygga target.  Kommandot är uppbyggt av namnet på det program som ska
exekveras följt av lista med argument till programmet.  Argumenten i listan
separeras av blanktecken.  Listan av argument avslutas med nyrad.

Blanktecken (förutom nyrad) har ingen betydelse så länge det inte är första
tecknet i en rad eller används för att separera element i en prerequisite eller
argument lista.

Target för regeln högst upp i filen kallas *default target* och är det som
kommer byggas ifall användaren av programmet inte specificerat vad som ska
byggas.

## Beteende

När *mmake* startas så kommer den ladda in alla regler från makefilen
*mmakefile* i nuvarande katalog eller filen som är specificerad med `-f`
flaggan.  Sedan kommer programmet försöka bygga de targets som specificerats som
argument till programmet eller default target.  Om användaren anger ett target
som inte existerar ska ett lämpligt felmeddelande skrivas ut.

*mmake* fungerar som en begränsad version av Unix kommandot *make*.  Programmet
använder filernas tidstämpel för senaste modifiering för att avgöra vilka delar av
programmet som behöver byggas om.  Den gör detta genom att titta på reglerna i
makefilen.  Ett target ska endast byggas ifall någon av följande tre situationer
gäller:

- Target filen existerar inte
- Tidpunkt då en prerequisite-fil modiferades är senare än tidpunkt då target-filen modiferades
- Flaggan `-B` används

Ifall ett target behöver byggas om exekveras kommandot i dess regel.  Om inte
`-s` flaggan används ska kommandot också skrivas ut på *standard output*.  Om
det finns en regel för någon av prerequisite-filerna ska  *mmake* utföra samma
logik rekursivt för att uppdatera prerequisite-filerna *innan* den avgör om den
nuvarande regeln behöver behöver utföras.

Ifall någon av prerequisite-filerna saknas och det inte finns någon regel för
att bygga den ska ett lämpligt felmeddelande skrivas på *standard error*.
Ifall ett kommando som exekveras misslyckas (inte returerar `EXIT_SUCCESS`) ska
*mmake* avslutas med `EXIT_FAILURE`.

## Hjälpmaterial till uppgiften

Till er hjälp tillhandahålls filerna [parser.c](./parser.c) och
[parser.h](./parser.h) som implementerar en parser av fil-formatet som ska
användas i lösningen.  Gränsytan till parsern är följande funktioner (som finns
dokumenterade i [parser.h](./parser.h)):

```c
makefile *parse_makefile(FILE *fp);

const char *makefile_default_target(makefile *make);
rule *makefile_rule(makefile *make, const char *target);

const char **rule_prereq(rule *rule);
char **rule_cmd(rule *rule);

void makefile_del(makefile *make);
```

Funktionen `parse_makefile` används för att parsa en öppnad makefil till en
`makefile` datastruktur.  Denna funktion returerar `NULL` om filen inte kunde
parsas.

Funktionen `makefile_default_target` returerar vad default target är för en
makefil.  Funktionen `makefile_rule` returerar en datastruktur med regeln för
att bygga ett target i makefilen eller `NULL` om en sådan regel inte existerar.

Funktionen `rule_prereq` returerar en `NULL`-terminerad array av filnamn som
är prerequisites för en regel.  Funktionen `rule_cmd` returerar en `NULL`-terminerad
array av argument för kommandot som ska exekveras för att utföra en regel, tänk
på att första argumentet är namnet av kommandot själv.

Funktionen `makefile_del` används för att frigöra minnet från en `makefile`
datastruktur.

## Begränsningar

Programmet behöver inte ha större noggrannhet än en sekund när det kommer till att
avgöra om en fil behöver byggas om.  Det behöver inte heller hantera cirkulära
beroenden i makefilen.  Eftersom det är en begränsad version av Make ska det
inte hantera saker som variabel-expansion eller wildcards.

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

Om ett anrop misslyckats ska programmet skriva ut ett lämpligt felmeddelande på
*standard error* för att sedan avsluta med returkoden `EXIT_FAILURE`. Om anropet
ändrar värdet på `errno` vid fel ska `perror` användas för att skriva ut
felmeddelandet.

## Rapport

Till uppgiften ska du inte skriva en fullständig rapport, men det du skriver
ska vara skrivet på ett vetenskapligt sätt.  Rapporten ska innehålla följande
delar:

- Kompilering och körning
- Användarhandledning

Kompilering och körning ska beskriva hur din lösning kompileras och körs via
terminal på institutionens datorer.  Detta bör beskrivas med exempel.

Användarhandledningen ska beskriva för en användare hur man använder
programmet.  Detta ska inkludera dokumentation över de flaggar som programmet
tar, filformat och vad programmet gör.  Även här kan man med fördel ha med
exempelkörningar för att underlätta beskrivningen.

På diskussion och reflektionsdelen kan du skriva om problem som du stötte på
när du gjorde laborationen, vad du tyckte om den och övriga åsikter som du vill
framföra.

## Övriga krav på lösningen

Utöver ovanstående nämnda beskrivning ska även lösningen uppfylla följande krav:

- Programmet ska använda sig av:
  - `stat`
  - `fork`
  - `execvp` (eller annan variant av `exec`)
  - `wait` (eller annan variant av `wait`)
- Programmet ska använda sig av getopt samt optind för att läsa in argument.
- Programmet får ej ha några minnesläckor (använd verktyget *valgrind*).
- Ändringar får inte göras i de tillhandahållna filerna.
- Inlämningen ska klara alla tester på Labres.
- Uppgiften ska lösas enskilt.

## Inlämning

Kod, makefile och rapport lämnas in via Labres innan utsatt deadline.

# Laboration 1 - `mexec`

I laborationen ska ni skriva ett C-program som kan exekvera en *pipeline*. En
pipeline är en sammansättning av program som körs parallellt där ett programs
utdata blir nästa programs indata.

Ett exempel på en sådan pipeline i **bash** är

```bash
cat -n mexec.c | grep -B4 -A2 return | less
```

som kommer att läsa och radnumrera innehållet i filen `mexec.c` för att sedan
leta reda på stycken i koden som innehåller ordet return. Slutligen skickar det
resultatet till less för enklare läsning av stora resultat.

I ert program kommer pipelinen att läsas in rad för rad istället för att *pipe*
tecknet '|' används. Samma pipeline kommer i ert fall vara:

```bash
cat -n mexec.c
grep -B4 -A2 return
less
```

## Gränsyta

Programmet ska heta *mexec* och indata ska kunna läsas in antingen direkt från
*standard input* eller via en angiven fil. Programmet ska endast acceptera noll
eller ett argument och om användaren skickar in fler argument ska ett lämpligt
felmeddelande skrivas ut och programmet ska då avsluta med returkod
`EXIT_FAILURE`.

### Inläsning från standard input

En fil omdirigeras till programmets *standard input* med hjälp av skalets `<`
operator, observera att ni ej behöver göra omdirigeringen själva utan att detta
görs av ert skal.

```bash
./mexec < file
```

Det går även att starta programmet utan en given indata med

```bash
./mexec
```

för att sedan skriva in kommandona rad för rad. Inläsning sker på exakt samma
sätt som då `<` operatorn används för att omdirigera *stdin* till en fil.  I
båda fallen kommer programmet att läsa från *stdin* tills dess att ett `EOF`
tecken läses in.

För att skicka ett `EOF` och avsluta inläsningen när alla kommandon är inmatade
använder du i de flesta terminaler `Ctrl-D`.

### Inläsning från fil

Programmet ska även kunna ta indata ifrån en fil som ges som första argument.
En fil skickas då in till programmet med

```bash
./mexec file
```

och det är upp till programmet att öppna den angivna filen för läsning.

### Indata

Formatet som ska stödjas för indata är:

- Varje kommando skrivs på en egen rad.
- Indata innhåller inga blanka rader.
- Argument till kommandon separeras med en eller flera blanksteg.
- Citattecken behöver ej stödjas, utgå från att varje argument är ett sammanhängande ord.
- Maximal radlängd är 1024 tecken.

## Utdata

Programmet i sig ska ej ge någon utdata på varken *standard input* eller
*standard error* vid en korrekt körning. Utdatan ska komma från de program som
exekveras.

## Kompilering

Programmet ska gå att kompilera utan varningar med hjälp av `make` och den
`Makefile` ni levererar tillsammans med programmet. Separatkompilering, som
även tar i hänsyn till alla eventuella header-filer, ska användas. Observera
att er `Makefile` ska skrivas manuellt och ej genereras av något verktyg.

Vid kompilering ska åtminstone följande flaggor användas för alla steg:

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
*standard error* för att sedan avsluta med returkoden `EXIT_FAILURE`. Om
anropet ändrar värdet på `errno` vid fel ska `perror` användas för att skriva
ut felmeddelandet.


### Kontroll av barnprocesser
När programmet väntar in barnprocesserna ska barnprocessernas returkod granskas
för att se om den är skild från 0, alltså att något fel har inträffat. Om någon
barnprocess returnerar ett värde skilt från 0 ska mexec returnera
`EXIT_FAILURE` vid avslut.

## Rapport

Till uppgiften ska du inte skriva en fullständig rapport, men det du skriver
ska vara skrivet på ett vetenskapligt sätt.  Rapporten ska innehålla följande
delar:

- Systembeskrivning av programmet
- Algorithmbeskrivning över algorithmen för att skapa pipor
- Diskussion och reflektion

Systembeskrivningen ska innehålla en översiktlig beskrivning av hur lösningen
är uppbyggd och ett anropsdiagram som visar vilka funktioner i programmet som
anropar vilka.

Algorithmbeskrivningen ska beskriva hur algorithmen som programmet använder för
att starta processor och bygga upp pipor fungerar.  Denna bör också inkludera
en figur som visar hur piporna byggs upp steg för steg.  Se lösningsförslag
från tidigare tentor för att se hur en sådan figur kan se ut.

På diskussion och reflektionsdelen kan du skriva om problem som du stötte på
när du gjorde laborationen, vad du tyckte om den och övriga åsikter som du vill
framföra.

## Övriga krav på lösningen

Utöver ovanstående nämnda beskrivning ska även programmet hålla sig till kraven:

- Programmet ska använda sig av:
  - `fork`
  - `pipe`
  - `close`
  - `dup2` (eller annan variant av `dup`)
  - `execvp` (eller annan variant av `exec`)
  - `wait` (eller annan variant av `wait`)
- Varje kommando ska läsa från föregående kommando i pipelinen, eller standard
  input för första kommandot.
- Varje kommando ska skriva till nästa kommando i pipelinen, eller standard
  output för sista kommandot.
- Alla kommandon ska köras parallellt.
- Programmet får ej ha några minnesläckor, använd `valgrind`.
- Programmet ska kunna köra ett godtyckligt antal kommandon och kan därför ej
  använda hårdkodade storlekar på datatyper. Använd `realloc` eller en dynamisk
  datatyp för att lagra data.
- Då programmet körs med en fil som indata får filen ej manipuleras under
  körningen.
- Koden ska skrivas med god kodkvalité (indentering, variabelnamn, kommentarer,
  funktionsuppdelning etc.).
- Programmets huvudfil ska heta *mexec.c*.
- Inlämningen ska klara alla tester på Labres.
- Uppgiften ska lösas enskilt.

## Testkörningar

Eftersom programmet, om det är fel, kan försöka skapa nya processer i en
oändlig loop ber vi er undvika testkörningar på `salt` och `peppar`. Istället
vill vi att ni kör programmet på någon av datorerna `itchy` eller `scratchy`.
Dessa två datorer är menade för testkörningar och det ska ej vara någon fara om
ett trasigt program körs där.

Ett tänkbart problem är att ni skapar så många processer att ni ej längre kan
göra något på er användare, och därmed ej kan stänga ned programmet. Vi önskar
därför att ni kör kommandot `ulimit -u 50` innan ni startar ert program. Detta
ställer om er `bash` session till att tillåta max 50 processer. Använder ni
något annat än `bash` får ni själva ta reda på hur ni gör motsvarande
inställning.

## Inlämning

Kod, makefile och rapport lämnas in via labres innan utsatt deadline.

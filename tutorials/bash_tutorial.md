# Bash Intro
Detta är en tutorial till bash /  
lista av trevliga kommandon man kan använda i bash / saker som gicks igenom på lektion 1. 
[Länk till gratis bok](https://linuxcommand.org/tlcl.php "bok :)") som mycket av detta är baserat på. Inte super detaljerad men väldigt bra! 

Notera att det mesta som tas upp här är **INTE** ett krav för kursen. 
Dock rekommenderar jag starkt att ni lär er lite av detta.
Är ni helt nya så är det dock väldigt väldigt mycket information/koncept att ta in.
Går att spendera väldigt många timmar här ifall ni läser manual sidorna noggrant / hittar på egna ideér. 
Så känn absolut inte att ni måste göra detta om ni har ont om tid pga labbar / andra kurser. 
Går alltid att spara filen och återkomma senare. Linux/Bash lär va relevant i allafall några år till...

Informationen är nog inte organiserad på det bästa sättet samt finns en massa mer jag skulle kunna ta upp. Ifall ni uppskattar detta / tycker det var lärorikt osv skicka gärna ett mail så vet jag ifall det är värt att lägga till mer tid på detta. Samt gärna ifall ni vill att jag ska täcka en specifik grej som ni undrar om.

## Grundläggande info
Ifall denna sektion är förvirrande i början så hoppa till sektion Praktisk genomgång 

Bash är shell-programmet som används by default i av de flesta Linux versioner/distrubitioner. 
Shell-program är ett alternativt sätt att kommunicera med datorn. (tillskillnad från att navigera grafiska interfaces) 
Dessa nås i de flesta operativsystem genom att öppna ett terminalprogram. 
(beroende på operativsystem/språk så sök efter terminal/commandprompt/kommandotolken)
I det simplaste fallet så skriver man in namnet på ett program man vill köra. (I en terminal som kör bash) 
Bash läser då in denna text, skapar en barnprocessen som execar detta program. 
Slutligen skriver bash ut resultatet av barnprocessens output i terminalen.

Bash ser normalt till så att barn-processens stdin är kopplad till terminalens input och dess stdout/stderr är kopplade till terminalens output.

Vill man koppla om stdin att läsa från en annan fil istället så skriver man (ersätt KOMMANDO med valfritt kommando vi går igenom nedan)
	```-> KOMMANDO < in_fil``` (-> står för er egen terminal prompt)
Vill man koppla om stdout att skriva ut till en annan fil istället så skriver man
	```-> KOMMANDO > ut_fil```
Går även att använda båda samtidigt. 
Kommandot nedan läser in data från fil1 och spottar ut resultatet i fil2. 
	```-> KOMMANDO < fil1 > fil2``` 
< och > kallas för redirection operators. 

Bash kan även köra flera kommandon samtidigt, när utdatan från kommando1 skickas som indata till kommando2. Syntax nedan. 
        ```-> KOMMANDO1 | KOMMANDO2```
Går att kedja väldigt många (max antal är ej definerat) kommandon på detta sätt. 
Men behövs oftast inte så många för vardagligt bruk. 
	```-> KOMMANDO1 | KOMMANDO2 | ... | KOMMANDO999```
Går även att kombinera redirection och pipes 
	```-> KOMMANDO1 < in_fil | KOMMANDO2 | KOMMANDO3 > ut_fil```
Konkret exempel, testa kör och se vad som händer. 
	```-> ls | grep a > a.txt ```
Kommer återkomma till mer avancerade bash-saker senare. Nu till lite praktisk användning. 

## Praktisk genomgång

Ifall ni inte vill glömma detta direkt så rekommenderar jag att följa med praktiskt. 
Så öppna en terminal!
Ifall den är väldigt ful så gå gärna in i settings och ändra textfärg/bakgrund osv. 
Mår dåligt av att se folk med en terminal som har vit bakgrund och svart text.

Logga in på skolans server (går även att göra från windows/mac. SSH är ett väldigt populärt protokoll.)
Har ni aldrig gjort detta så kommer SSH ge en varning.
Kortfattat så säger den bara att ni aldrig anslutit till denna server förrut och vill att ni ska vara försiktiga.
Så skriv bara yes/godkänn servern.
	```-> ssh CS-ANVÄNDARNAMN@scratchy.cs.umu.se```
För att se manual sida för alla kommandon/program/funktioner/osv på linux. Tryck q för quit. 
        ```-> man exempelkommando  ```
	```-> man man ```
	```-> man intro ```
	```-> man string```
Det finns även andra manual sidor, typ tldr. Går även att ladda ner andra från internet. 
Problemet med många av dessa är dock att de bara ger dig kommandona utan någon förklaring.
Samt att de ofta inte finns like många. Finns för tillfället 10k+ man pages på linux. 

För att se din plats i filträdet, alltså vilken directory du är i 
	```-> pwd ``` (print working directory) 
	```-> man pwd ```
För att se vad som ligger i denna directory 
	```-> ls ```
	```-> ls -l ``` (ger mer info om alla entries)
	```-> ls -a ``` (visar även gömda filer)
	```-> man ls```
För att byta directory 
	```-> cd nymap  ``` (baserar då vart du går baserat på vart du var innan.)
	```-> cd ```(går tillbaka till homedirectory)
	```-> cd ~ ```(samma som ovan) 
	```-> cd / ```(går till grunden i filsystemet)
	```-> cd /nått/nått/osv ``` (navigerar baserat på absolute path)
	```-> cd . ``` (hoppa till nuvarande directory
	. är bara en referens som finns i varje directory som refererar till sig själv) 
	```-> cd .. ``` (hoppar till directoryn bakom, samma koncept som .) 
	```-> man cd```
Förklaring på vad alla directories gör i Linux
	```-> man hier ```
	```-> cd /usr/include ```(här ligger alla include files typ stdlib.h) 
	```-> cd /proc/(ID)/fd ``` (alla öppna fildescriptorer för processen). 
	```-> cd /bin ``` (innehåller de flesta program.) 
För att se en trädstruktur över vart vi är 
	```-> tree ```
	```-> man tree ```
För att skapa en ny map 
	```-> mkdir testmap ```
	```-> man mkdir ```
Kanske börjar bli jobbigt att köra man på varje program.
Poängen är att ni ska bli vana att göra det.  
När man väl kan läsa dessa (vilket tar ett tag) går det sjukt snabbt att leda reda på information tillskillnad på att tex googla. 
Tillexempel hur var nu syntaxen för att läsa in en rad igen? 
	```-> man 3 getline ```
ta bort en map 
	```-> rmdir testmap ```
	```-> man rmdir ```
skapa fil 
	```-> touch newfile ```
	```-> man touch ``` (finns inget officielt skapa fil program)
	```-> > newfile ``` (fungerar också)
ta bort filen  
	```-> rm newfile ``` (var försiktiga med rm, med vissa options (typ -r) kan man råka ta bort många filer.) 
	```-> man ```
De flesta kommandon fungerar precis som cd på både relativ och absolute path.  
så vill man skapa en fil i bas directoryn 
	```-> touch /newfile ``` (krävs dock sudo access (aka typ admin access) som ni inte har.) 
skapa fil i en folder som exister i den foldern ni är i nu 
	```-> touch folder/newfile```
kolla chars, words, lines i en fil. 
	```-> wc fil ```
	```-> man wc ``` (testa gärna lite options här på wc då dem är rätt simpla) 
kolla annan info om fil 
	```-> stat fil```
	```-> man stat```

Git som ni troligtvis stött på förrut går såklart att köra från terminalen.
För att ladda ner kursens repo
	```-> git clone git@git.cs.umu.se:courses/5dv088ht21.git (både gitlab/github har en färggrann knapp med ssh/http länk om ni vill ladda ner andra repos).```
	```-> man gittutorial ```(går igenom alla andra git grejer bättre än vad jag kan)

Använda parser programmet pandoc för att konvertera en markdown fil till vanlig text.
Kan parsa de mesta filtyper så rätt coolt å veta om. 
	```-> pandoc bash_tutorial.md -o bash_tutorial.txt --to=plain ``` (kräver att ni laddat ner filen/är i rätt directory obv)

Söka igenom man sidorna för keywords 
	```-> apropos keyword```
	```-> man apropos ```
Sök igenom text och ge tillbaka rader som innehåller keyword
	```-> grep keyword ```
	```-> man grep ```
apropos och grep går att kombinera för rätt effektiv sökning om man har ett humm om vad man letar efter. 
	```-> apropos make | grep directory ```(ger hyffsat vettigt svar) 
	```-> apropos create | grep dir ```(även detta)
för att söka igenom filträdet efter en fil med specifikt namn  
ge tillbaka sökvägen till dessa
	```-> find dir_att_söka_ifrån -name namn_på_fil ```
	```-> find ~ -name mexec ```(ger tillbaka den absoluta sökvägen till alla filer som heter mexec och ligger i er hemmap). 
	```-> man find ```
find kan göra ganska avancerade grejer. Ni kan kolla upp själva om ni är intresserade. 

Skriv ut er commandhistory (går även att ändra hur många rader kommandot sparar (testa söka upp hur detta fungerar själva)) 
	```-> history ```(läser egentligen bara igenom filen ~.bash_history) 
	```-> history | grep ```(vettigt kommando om ni söker efter ett gammalt kommando ni kunnat syntaxen för tidigare, går även att använda ctrl+R i bash för liknande) 
	```-> man history ```
	```-> !37 ```(kör kommando 37 i history) 
Använd chmod för att ändra permissions på filer (vem som har rättigheter att läsa/skriva i dem osv.) 
rättigheterna går att se med 
	```-> ls -l ```
går att ändra rättigheterna med chmod på olika sätt, förklarar ett. 
nedan står u för user, g för group, o för others, r för read, w för write, x för execute. 
	```-> chmod u=rwx, g=r, o=r exempel.fil ```
	```-> man chmod ```
Testa på en fil och kolla sedan med ```ls -l ``` igen för att se hur permissions ändras. 

Kolla alla processer som är kopplade till terminalen 
	```-> ps ```
	```-> man ps ```
Kolla alla processer 
	```-> ps -e ```
Träd av alla processer (vilka som skapat vilka osv.) 
	```-> pstree ```
Skicka en signal till en process baserat på dess pid (väldigt brutalt kommando namn) 
	```-> kill -signalnummber pid ```
	```-> kill -9 4222 ```(skickar signalen SIGKILL till processen med id 4222)
	```-> man kill```
Testa gärna döda er bash session. 
se även man 7 signal för andra signaler. 

För att edita filer i terminalen kan man använda sig alla olika program. visar två här. 
	```-> nano testfile ```(^X står för Ctrl+x)
En lite mer avancerad är vim. kör dock först lite av vimtutor innan ni öppnar vim på riktigt. 
	```-> vimtutor```
	```-> vim fil```

Starta program i bakgrunden (aka låt inte programmet ta över terminalens stdin) 
	```-> program & ```

## Enviroment variables 
Som ni kanske vet har alla program nått som kallas enviroment variables. 
Vissa versioner av exec behandlar dessa. 

Går att accessa i en c fil genom att deklarera variabeln ```extern char **environ;```
Finns några kapitel i boken om detta om ni vill förstå detaljerna. 
Iallafall så är det bara ett gäng textsträngar.
Men bash använder sig mycket utav några sådana.
En utav dem är PS1. 
	```-> echo $PS1 ```(skriver ut värdet på PS1, lär mest bara se ut som kaos).
Den kontrollerar hur eran command promt ser ut.
	```-> PS1="bash_expert_nr1-> "```
Coolt right??
Startar ni dock om terminalen så är ni tillbaka till er tråkiga PS1.
Detta är för att bash har en/flera setupfiler som läses av när bash startas.
Er användarsetupfil är ~.bashrc eller ~.bash_profile
	```-> vim/nano ~.bashrc```
Skriv på vilken tom rad som helst:
```PS1="\[\033[0;36m\]\# \w-> \[\033[0m\] "```
Spara filen, starta om bash/terminalen så ser ni vad kommandot gör.
Finns massa coola grejer, kolla kapitel 13 i boken länkad längst upp för mer.

En annan enviromentvariabel som bash ofta använder sig av är $PATH
	```-> echo $PATH```
Den indikerar vilken path/directories bash ska söka igenom när den letar efter kommandon att köra.
(separerade med :   )
Så när ni skriver in kommandot
	```-> ls```
Letar bash stegvis igenom dessa directories tills de hittar ett program som heter ls och exekverar det. PATH variabeln går att ändra hur ni vill. 

Röra clipboard via terminalen
	-> man xclip
	-> echo "wtf" | xclip -i -selection clipboard
Testa ctrl+v i webbläsaren eller ctrl+shift+v i terminalen.
</p>

## Kul (D:) Uppgift

Ni skulle tillexempel kunna sätta upp en directory i er användere 
	```-> mkdir ~/bin```
Lägg till den i er PATH
Skriv en makefile som alltid (eller via specifik regel) skickar resultatet av när ni kompilerar era c program till ~/bin
Då slipper ni skriva ./program för att köra dem / kan köra dem från vilken directory som helst.
Detta kräver att ni lär er makefiles bra så se det som en trevlig belöning.
Bra tutorial till make
https://makefiletutorial.com/
Kom ihåg att make bara är ett program som ligger i /bin

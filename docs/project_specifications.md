Tilgjengeliggjort 30. september



Prosjekt: "Bilsimulator"



Dere skal lage et program der en bil/tanks e.l. kan styres i et 3D-miljø.



Kravspesifikasjon

Dere skal skrive programvare som benytter seg av biblioteket threepp. Dette er et bibliotek som gjør det mulig å tegne og interagere med 3D objekter på skjermen, samt hente input fra bruker gjennom mus og tastatur. Det er også mulig å lage enkle brukergrensesnitt ved hjelp av ekstra biblioteker som f.eks. imgui.



Koden skal skrives i moderne C++ (17/20) og bruke CMake.





Funksjonalitet



Bilkontroll

Bevegelse fremover/bakover.

Rotasjon.

Enkel kollisjonsdeteksjon mot objekter.

Noe som viser at bilen er i bevegelse (hjul rotasjon etc.)



Objekt interaksjon

Når bilen kommer nær visse objekter, så kan det "plukkes opp".

Dette kan være "power-ups" som endrer størrelse, hastighet etc., eller nøkkelelementer som trengs for å gjøre noe. F.eks åpne en dør.



Miljø

Flatt plan.

Spredte objekter som kan plukkes opp.

Hindringer, fare-elementer, dører, portaler o.l.



Bonus

Bruk av lyd.

UI elementer.

Dynamikk i styringen.

Dvs. Modeller akselerasjon, momentum, friksjon etc.

Simulering av sensorer.

Fargesensor.

Distansesensor.

Lidar.

Kamera.



Avansert

Integrer applikasjonen med bildebehandlings biblioteker som OpenCV.

Integrer applikasjonen med en fysikkmotor.

Linjefølgemodus.

Denne ligger under avansert fordi man må bruke avanserte konsepter eller tenke utenfor boksen når det gjelder å simulere sensoren.





Annet

Det bør være mulig å resette applikasjonen (starte på nytt) uten at bruker må avslutte og starte applikasjonen på nytt.

Dere må gjerne inkludere 3D modeller, teksturer og lignende, men tidsbruk i modelleringsverktøy som Blender teller egentlig ikke i evalueringen (men god estetikk kan gi godt inntrykk).

Prøv i så stor grad som mulig å skille logikk fra visualisering. Dette henger også sammen med designprinsippene coupling og cohesion.





Kildehenvisninger

Dersom dere i merkbar grad benytter dere av kode som dere selv ikke har skrevet, skal dette dokumenteres i kodebasen sammen med henvisning til kilden.



Versjonskontroll/Git

Det forventes at versjonseringsverktøyet Git benyttes, og at prosjekthjelpemidler som issues og pull requests benyttes i prosjektet.



Enhetstester

Prosjektet skal inneholde enhetstester (unit-tests) som kan teste ulike deler av koden. Catch2 brukt i øvinger og etterhvert demonstrert i klassen oppfordres til å brukes.



Bruk av tredjeparts bibliotek

Dere står fritt til å bruke tredjeparts bibliotek, men disse må være konfigurert på en slik måte at det er mulig for andre å trivielt bygge koden. Dette gjelder også bruk av rammeverk for enhetstesting.



Continious Integration (CI)

Automatisk bygging samt testing ved bruk av github workflows vil bidra til en bedre løsning som gjør det mulig å sjekke kryssplatforms-kompabilitet. Ta gjerne utgangspunkt i threepp sitt eget CI oppsett, men husk å krediter kilden som med all annen kode dere finner/gjenbruker.







Evaluering

Besvarelser vil bli vurdert basert på følgende kriterier:

Om kravspesifikasjonen har blitt fulgt.

Om god forståelse for grunnleggende prinsipper i programmering som variabler, datatyper, funksjoner, klasser, metoder, betingelser, løkker osv. kan demonstreres.

Fornuftig design og bruk av klasser og funksjoner.

Fornuftig bruk av datastrukturer.

Om prinsippene for god design (abstraction, encapsulation, coupling, cohesion, responsibility driven design osv.) har blitt fulgt.

Om det er brukt gode, selvforklarende navn på klasser, metoder og variabler/felt/parametere.

Riktig og ellers fornuftig bruk av referanser, pekere og «value types».

Bruk av konsekvent og rydding kode design (formatering, konvensjoner etc.).

Om prosjektet er oversiktlig og godt strukturert.

Om koden er selvforklarende og ellers godt dokumentert.

Om gode og dekkende enhetstester er implementert.

Tilstrekkelig feilhåndtering («defensive programming», evt. håndtering av ugyldig input fra bruker, verifisering av parametere osv.).

Gjennomgående bruk av moderne C++.

Evne til å reflektere over løsningen.

Kreativitet og selvstendighet.







Sett deg raskt inn i prosjektoppgaven og planlegg tidsbruken.

Bruk Lab effektivt.





Besvare og levere

Den endelige mappen leveres individuelt i Inspera. Denne skal bestå av:

Endelige prosjektfiler (Kode, CMake konfigurasjon, README, evt. ressurser, etc.)

Det forventes at prosjektet har en godt beskrivende README fil (markdown syntax) som beskriver prosjektet.

Hva er dette?

Hva kan det gjøre?

Hvordan skal det brukes?

Simplifisert UML klassediagram av egendefinerte typer.

Få med kandidatnummer i README.

Benytt README også til å reflektere over prosjektet.

Hva er du fornøyd med og hvorfor.

Hva har forbedringspotensial og hvordan kunne du kunne ha håndtert det bedre (vis teknisk innsikt).

NB! Maskinspesifikke konfigurasjons- og byggfiler (cmake-build-*, .idea, etc.) skal ikke være med.



Dette skal leveres i en enkelt komprimert mappe (.zip).

DOBBELTSJEKK at riktige filer har blitt lastet opp!





https://i.ntnu.no/wiki/-/wiki/Norsk/Karakterbeskrivelser+for+teknologiske+fag





--------------------------------------------------------------------------- 



Tips for raskere threepp kompilering (hovedsakelig for MinGW brukere):



Set "–DBUILD_SHARED_LIBS=ON" i CMake options (Settings->CMake)



NB!

Når du setter SHARED_LIBS=ON bygges ikke bibliotekene du bruker inn i den kjørbare fila. Dem får heller sin egen .dll fil (.so på linux, .dylib på MacOS) som dynamisk lastes inn når du kjører fila.  Du må da passe på at den kjørbare fila kan finne disse filene. F.eks ved at dem ligger i samme mappe.



Dette kan ordnes via CMake ved å legge inn i CMakeLists.txt:

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") 

 
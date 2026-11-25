# Flight Planner v0.1.0

Desktop aplikacija zasnovana na natID SDK-u. Omogućava unos letova, pregled i uklanjanje unosa, rangiranje mogućih povratnih putovanja te čuvanje i učitavanje podataka.

## Funkcionalnosti

- Unos polaznih i povratnih letova sa datumom, lokacijama i cijenom u EUR.
- Dodatni troškovi i ukupne cijene računaju se u EUR.
- Filtriranje i rangiranje putovanja po cijeni ili dužini boravka.
- Čuvanje i učitavanje letova kroz meni **Letovi**.
- Interfejs je preveden na bosanski, engleski, francuski, latvijski i švedski.
- Windows taskbar ikona je u `res/PNG-like/ikonica.ico`.

## Potrebno za izradu

- CMake 3.18 ili noviji.
- C++20 kompajler. Na Windowsu se preporučuje Visual Studio sa instaliranim C++ desktop razvojnim alatima.
- natID SDK, koji se ne isporučuje u ovom repozitoriju i mora biti dostupan zasebno.

CMake podrazumijevano traži SDK u `%USERPROFILE%\\natID.SDK` na Windowsu, odnosno `$HOME/natID.SDK` na Linuxu i macOS-u. Drugu lokaciju možeš navesti kroz `NATID_SDK_ROOT`.

## Izrada

Iz osnovnog direktorija repozitorija pokrenuti:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Ako SDK nije u podrazumijevanoj lokaciji:

```powershell
cmake -S . -B build -DNATID_SDK_ROOT="C:/putanja/do/natID.SDK"
cmake --build build --config Release
```

## Podaci

Baza je tekstualna datoteka u korisničkom direktoriju podataka: na Windowsu `%LOCALAPPDATA%\FlightPlannerNatID\flights.txt`, na macOS-u `~/Library/Application Support/FlightPlannerNatID/flights.txt`, a na Linuxu `$XDG_DATA_HOME/FlightPlannerNatID/flights.txt` ili `~/.local/share/FlightPlannerNatID/flights.txt`. Aplikacija prvo traži podatke na toj novoj lokaciji, a pri učitavanju podržava i raniju putanju `../data/flights.txt`.

- **Sačuvaj** prepisuje datoteku letovima koji su trenutno u aplikaciji.
- **Učitaj** dodaje letove iz datoteke postojećim letovima u aplikaciji. Ponovljeno učitavanje iste datoteke ponovo dodaje iste letove.
- Svaki red sadrži smjer (`D` ili `R`), datum u ISO formatu, polaznu i odredišnu lokaciju pod navodnicima te cijenu karte i dodatni trošak.

## Poznata ograničenja v0.1

- Promjena jezika u trenutno aktivnom prozoru još nije riješena; izabrani jezik postaje vidljiv nakon ponovnog pokretanja aplikacije.
- Ako je datoteka neispravna, učitavanje se odbija bez djelimičnog dodavanja letova.
- Projekat koristi natID SDK koji se ne isporučuje u ovom direktoriju.

## Resursi

`ikonica.ico` se koristi za Windows aplikaciju. `ikonicaVec.svg` i `image.png` su dodatni vizuelni resursi. PNG ilustracija nije postavljena u glavni unosni ekran jer bi zauzimala prostor potreban kontrolama; može se iskoristiti na budućem početnom ili „O aplikaciji“ prikazu.

## Izdanje instalacije

Na Windowsu su potrebni Visual Studio C++ alati, CMake 3.18+, Inno Setup 6 ili 7 i natID SDK. Jednom skriptom se gradi Release aplikacija, staging direktorij i instalacijski paket:

```powershell
powershell -ExecutionPolicy Bypass -File installer/build-setup.ps1
```

Ako SDK nije u `%USERPROFILE%\natID.SDK`, navedi njegov direktorij:

```powershell
powershell -ExecutionPolicy Bypass -File installer/build-setup.ps1 -NatIdSdkRoot "C:/putanja/do/natID.SDK"
```

Skripta preuzima Microsoft Visual C++ x86 runtime sa zvanične Microsoft adrese i provjerava digitalni potpis prije nego što ga uključi. Staging uključuje Release DLL-ove natID/GTK, GTK runtime podatke, natID `Common` resurse, aplikacijske prevode i pokretač koji postavlja potrebne putanje. Instalacijski paket se pravi u `installer-output/FlightPlanner-Setup-0.1.0.exe`. Izlazne build datoteke i setup se ne čuvaju u izvornom kodu; setup treba dodati kao asset uz GitHub Release.

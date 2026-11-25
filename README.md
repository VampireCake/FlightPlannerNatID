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

Baza je tekstualna datoteka `../data/flights.txt`, relativno prema direktoriju projekta. Pokretanje iz Visual Studija koristi direktorij projekta kao radni direktorij.

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

Izlazne build datoteke i instalacijski paket ne čuvaju se u izvornom kodu. Kada se pripremi setup, preporučeno je dodati ga kao asset uz GitHub Release, uz odgovarajuću verziju aplikacije.

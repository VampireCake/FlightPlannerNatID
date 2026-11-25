#include "TripRanker.h"

#include <algorithm>
#include <cctype>

namespace
{
    std::string normalizujLokaciju(const std::string& lokacija)
    {
        std::string rezultat;
        bool prethodniJeRazmak = true;

        for (std::size_t i = 0; i < lokacija.size(); ++i)
        {
            unsigned char znak = static_cast<unsigned char>(lokacija[i]);
            if (std::isspace(znak))
            {
                if (!prethodniJeRazmak)
                {
                    rezultat += ' ';
                    prethodniJeRazmak = true;
                }
            }
            else
            {
                rezultat += static_cast<char>(std::tolower(znak));
                prethodniJeRazmak = false;
            }
        }

        if (!rezultat.empty() && rezultat[rezultat.size() - 1] == ' ')
            rezultat.erase(rezultat.size() - 1);

        return rezultat;
    }

    bool istaLokacija(const std::string& prva, const std::string& druga)
    {
        return normalizujLokaciju(prva) == normalizujLokaciju(druga);
    }

    bool prijePoDatumima(const OpcijaPutovanja& prva,
                         const OpcijaPutovanja& druga,
                         const std::vector<Let>& letovi)
    {
        const td::Date& prviPolazak = letovi[prva.indeksPolaska].getDatum();
        const td::Date& drugiPolazak = letovi[druga.indeksPolaska].getDatum();

        if (prviPolazak != drugiPolazak)
            return prviPolazak < drugiPolazak;

        const td::Date& prviPovratak = letovi[prva.indeksPovratka].getDatum();
        const td::Date& drugiPovratak = letovi[druga.indeksPovratka].getDatum();
        return prviPovratak < drugiPovratak;
    }
}

FilterPutovanja::FilterPutovanja()
    : koristiNajranijiPolazak(false)
    , koristiNajkasnijiPovratak(false)
    , minimalanBrojNocenja(0)
    , maksimalanBrojNocenja(365)
    , koristiMaksimalnuCijenu(false)
    , maksimalnaCijenaEUR(0.0)
    , nacinRangiranja(NacinRangiranja::NajnizaCijena)
{
}

std::vector<OpcijaPutovanja> RangiracLetova::rangiraj(
    const std::vector<Let>& letovi,
    const FilterPutovanja& filter)
{
    std::vector<OpcijaPutovanja> opcije;
    const std::string trazeniGrad = normalizujLokaciju(filter.gradSusreta);

    for (std::size_t i = 0; i < letovi.size(); ++i)
    {
        const Let& polazak = letovi[i];
        if (polazak.getTip() != TipLeta::Polazak)
            continue;

        for (std::size_t j = 0; j < letovi.size(); ++j)
        {
            const Let& povratak = letovi[j];
            if (povratak.getTip() != TipLeta::Povratak)
                continue;

            // Povratni let mora polaziti iz grada u koji je putnik stigao.
            if (!istaLokacija(polazak.getLokacijaSlijetanja(),
                              povratak.getLokacijaPolijetanja()))
                continue;

            if (!trazeniGrad.empty() &&
                !istaLokacija(polazak.getLokacijaSlijetanja(), filter.gradSusreta))
                continue;

            if (povratak.getDatum() < polazak.getDatum())
                continue;

            if (filter.koristiNajranijiPolazak &&
                polazak.getDatum() < filter.najranijiPolazak)
                continue;

            if (filter.koristiNajkasnijiPovratak &&
                povratak.getDatum() > filter.najkasnijiPovratak)
                continue;

            td::Date kopijaPovratka = povratak.getDatum();
            td::Date kopijaPolaska = polazak.getDatum();
            const int brojNocenja = kopijaPovratka - kopijaPolaska;

            if (brojNocenja < filter.minimalanBrojNocenja ||
                brojNocenja > filter.maksimalanBrojNocenja)
                continue;

            const double ukupnaCijena =
                polazak.getUkupnaCijenaEUR() + povratak.getUkupnaCijenaEUR();

            if (filter.koristiMaksimalnuCijenu &&
                ukupnaCijena > filter.maksimalnaCijenaEUR)
                continue;

            OpcijaPutovanja opcija;
            opcija.indeksPolaska = i;
            opcija.indeksPovratka = j;
            opcija.brojNocenja = brojNocenja;
            opcija.ukupnaCijenaEUR = ukupnaCijena;
            opcije.push_back(opcija);
        }
    }

    std::stable_sort(opcije.begin(), opcije.end(),
        [&letovi, &filter](const OpcijaPutovanja& prva,
                           const OpcijaPutovanja& druga)
        {
            if (filter.nacinRangiranja == NacinRangiranja::NajduziBoravak)
            {
                if (prva.brojNocenja != druga.brojNocenja)
                    return prva.brojNocenja > druga.brojNocenja;
                if (prva.ukupnaCijenaEUR != druga.ukupnaCijenaEUR)
                    return prva.ukupnaCijenaEUR < druga.ukupnaCijenaEUR;
            }
            else if (filter.nacinRangiranja == NacinRangiranja::NajkraciBoravak)
            {
                if (prva.brojNocenja != druga.brojNocenja)
                    return prva.brojNocenja < druga.brojNocenja;
                if (prva.ukupnaCijenaEUR != druga.ukupnaCijenaEUR)
                    return prva.ukupnaCijenaEUR < druga.ukupnaCijenaEUR;
            }
            else
            {
                if (prva.ukupnaCijenaEUR != druga.ukupnaCijenaEUR)
                    return prva.ukupnaCijenaEUR < druga.ukupnaCijenaEUR;
                if (prva.brojNocenja != druga.brojNocenja)
                    return prva.brojNocenja > druga.brojNocenja;
            }

            return prijePoDatumima(prva, druga, letovi);
        });

    return opcije;
}

#pragma once

#include "Let.h"
#include <cstddef>
#include <string>
#include <vector>

enum class NacinRangiranja
{
    NajnizaCijena,
    NajduziBoravak,
    NajkraciBoravak
};

struct FilterPutovanja
{
    std::string gradSusreta;

    bool koristiNajranijiPolazak;
    td::Date najranijiPolazak;

    bool koristiNajkasnijiPovratak;
    td::Date najkasnijiPovratak;

    int minimalanBrojNocenja;
    int maksimalanBrojNocenja;

    bool koristiMaksimalnuCijenu;
    double maksimalnaCijenaEUR;

    NacinRangiranja nacinRangiranja;

    FilterPutovanja();
};

struct OpcijaPutovanja
{
    std::size_t indeksPolaska;
    std::size_t indeksPovratka;
    int brojNocenja;
    double ukupnaCijenaEUR;
};

class RangiracLetova
{
public:
    static std::vector<OpcijaPutovanja> rangiraj(
        const std::vector<Let>& letovi,
        const FilterPutovanja& filter);
};

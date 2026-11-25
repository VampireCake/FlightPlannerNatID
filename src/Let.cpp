#include "Let.h"

Let::Let(const std::string& lokacijaPolijetanja,
         const std::string& lokacijaSlijetanja,
         const td::Date& datum,
         double cijenaEUR,
         double dodatniTrosakEUR,
         TipLeta tip)
    : _lokacijaPolijetanja(lokacijaPolijetanja)
    , _lokacijaSlijetanja(lokacijaSlijetanja)
    , _datum(datum)
    , _cijenaEUR(cijenaEUR)
    , _dodatniTrosakEUR(dodatniTrosakEUR)
    , _tip(tip)
{
}

const std::string& Let::getLokacijaPolijetanja() const
{
    return _lokacijaPolijetanja;
}

const std::string& Let::getLokacijaSlijetanja() const
{
    return _lokacijaSlijetanja;
}

const td::Date& Let::getDatum() const
{
    return _datum;
}

double Let::getCijenaEUR() const
{
    return _cijenaEUR;
}

double Let::getDodatniTrosakEUR() const
{
    return _dodatniTrosakEUR;
}

bool Let::imaDodatneTroskove() const
{
    return _dodatniTrosakEUR > 0.0;
}

TipLeta Let::getTip() const
{
    return _tip;
}

double Let::getUkupnaCijenaEUR() const
{
    return _cijenaEUR + _dodatniTrosakEUR;
}

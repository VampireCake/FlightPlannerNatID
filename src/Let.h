#pragma once

#include <string>
#include <td/Date.h>

enum class TipLeta
{
    Polazak,
    Povratak
};

class Let
{
private:
    std::string _lokacijaPolijetanja;
    std::string _lokacijaSlijetanja;
    td::Date _datum;
    double _cijenaEUR;
    double _dodatniTrosakEUR;
    TipLeta _tip;

public:
    Let(const std::string& lokacijaPolijetanja,
        const std::string& lokacijaSlijetanja,
        const td::Date& datum,
        double cijenaEUR,
        double dodatniTrosakEUR,
        TipLeta tip);

    const std::string& getLokacijaPolijetanja() const;
    const std::string& getLokacijaSlijetanja() const;
    const td::Date& getDatum() const;
    double getCijenaEUR() const;
    double getDodatniTrosakEUR() const;
    bool imaDodatneTroskove() const;
    TipLeta getTip() const;

    double getUkupnaCijenaEUR() const;
};

#include "FlightPlannerView.h"

#include <gui/Application.h>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <td/String.h>

namespace
{
    constexpr char LegacyFlightDataFilePath[] = "../data/flights.txt";

    std::filesystem::path getFlightDataFilePath()
    {
#if defined(_WIN32)
        if (const char* appData = std::getenv("LOCALAPPDATA"); appData && *appData)
            return std::filesystem::path(appData) / "FlightPlannerNatID" / "flights.txt";
#elif defined(__APPLE__)
        if (const char* home = std::getenv("HOME"); home && *home)
            return std::filesystem::path(home) / "Library" / "Application Support" /
                   "FlightPlannerNatID" / "flights.txt";
#else
        if (const char* xdgData = std::getenv("XDG_DATA_HOME"); xdgData && *xdgData)
            return std::filesystem::path(xdgData) / "FlightPlannerNatID" / "flights.txt";
        if (const char* home = std::getenv("HOME"); home && *home)
            return std::filesystem::path(home) / ".local" / "share" /
                   "FlightPlannerNatID" / "flights.txt";
#endif
        return std::filesystem::path(LegacyFlightDataFilePath);
    }

    std::string asStdString(const td::String& value)
    {
        return std::string(value.c_str());
    }

    std::string trim(const std::string& value)
    {
        std::size_t first = 0;
        while (first < value.size() &&
               std::isspace(static_cast<unsigned char>(value[first])))
            ++first;

        std::size_t last = value.size();
        while (last > first &&
               std::isspace(static_cast<unsigned char>(value[last - 1])))
            --last;

        return value.substr(first, last - first);
    }

    std::string formatDate(const td::Date& date)
    {
        std::ostringstream output;
        output << std::setfill('0')
               << std::setw(2) << date.getDay() << "."
               << std::setw(2) << date.getMonth() << "."
               << std::setw(4) << date.getYear();
        return output.str();
    }

    bool isDateSet(const td::Date& date)
    {
        return date.getYear() > 0 &&
               date.getMonth() >= 1 && date.getMonth() <= 12 &&
               date.getDay() >= 1 && date.getDay() <= 31;
    }

    std::string formatMoney(double amount)
    {
        std::ostringstream output;
        output << std::fixed << std::setprecision(2) << amount;
        return output.str();
    }
}

FlightPlannerView::FlightPlannerView()
    : _layout(1, 1)
    , _hasRanked(false)
{
    setBackgroundColor(td::ColorID::SysBackAlt1);

    _flightPage = std::make_unique<FlightEntryPage>(this);
    _tripPage = std::make_unique<TripPlannerPage>(this);
    _flightPage->setTitle(tr("manageFlightsView"));
    _tripPage->setTitle(tr("findTripsView"));
    _tabs.addView(_flightPage.get(), tr("manageFlightsView"));
    _tabs.addView(_tripPage.get(), tr("findTripsView"));

    _layout.setMargins(0, 0);
    _layout.insert(0, 0, _tabs,
                   td::HAlignment::Left, td::VAlignment::Top);
    setLayout(&_layout);
}

void FlightPlannerView::systemColorModeChanged(bool bDarkMode)
{
    gui::View::systemColorModeChanged(bDarkMode);
    setBackgroundColor(td::ColorID::SysBackAlt1);
}

void FlightPlannerView::applyLanguage()
{
    _flightPage->setTitle(tr("manageFlightsView"));
    _tripPage->setTitle(tr("findTripsView"));
    _flightPage->applyLanguage();
    _tripPage->applyLanguage();
    setLayout(&_layout);
}

void FlightPlannerView::changeLanguage(const td::String& extension)
{
    auto* app = getApplication();
    if (!app)
        return;

    app->getSettings()->setTranslationExtension(extension);
    app->getProperties()->setValue("translation", extension);
    app->initRegionals(extension);
    app->setLanguage(extension);
    applyLanguage();
}

void FlightPlannerView::setDefaultSortOrder(int sortIndex)
{
    _tripPage->setDefaultSortOrder(sortIndex);
}

void FlightPlannerView::saveFlights()
{
    _flightPage->saveFlights();
}

void FlightPlannerView::loadFlights()
{
    _flightPage->loadFlights();
}

void FlightPlannerView::invalidateRankedTrips()
{
    _rankedOptions.clear();
    _hasRanked = false;
    _tripPage->clearResults();
}

void FlightPlannerView::showPage(int pageIndex)
{
    if (pageIndex < 0 || pageIndex > 1)
        return;

    _tabs.setCurrentViewPos(pageIndex);
}

FlightEntryPage::FlightEntryPage(FlightPlannerView* owner)
    : _owner(owner)
    , _flightHeader(tr("newFlight"))
    , _directionLabel(tr("flightType"))
    , _flightDateLabel(tr("flightDate"))
    , _originLabel(tr("departureLocation"))
    , _destinationLabel(tr("arrivalLocation"))
    , _fareLabel(tr("ticketPrice"))
    , _fareEUR(td::decimal2)
    , _extraCostLabel(tr("extraCost"))
    , _extraCostEUR(td::decimal2)
    , _addFlightButton(tr("addFlight"))
    , _showFlightsButton(tr("showFlights"))
    , _flightListHeader(tr("enteredFlights"))
    , _flightToRemoveLabel(tr("flightToRemoveLabel"))
    , _removeFlightButton(tr("removeSelectedFlight"))
    , _removeBarLayout(3)
    , _layout(10, 4)
{
    gui::DateEdit::putLeadingZeros(true);

    _addFlightButton.setType(gui::Button::Type::Constructive);
    _removeFlightButton.setType(gui::Button::Type::Destructive);
    _removeFlightButton.onClick([this] { removeSelectedFlight(); });

    _direction.addItem(tr("departure"));
    _direction.addItem(tr("return"));
    _direction.selectIndex(0);

    _directionLabel.setSizeLimitForNChars(18, gui::Control::Limit::UseAsMin);
    _flightDateLabel.setSizeLimitForNChars(18, gui::Control::Limit::UseAsMin);
    _originLabel.setSizeLimitForNChars(18, gui::Control::Limit::UseAsMin);
    _destinationLabel.setSizeLimitForNChars(18, gui::Control::Limit::UseAsMin);
    _fareLabel.setSizeLimitForNChars(18, gui::Control::Limit::UseAsMin);
    _extraCostLabel.setSizeLimitForNChars(18, gui::Control::Limit::UseAsMin);
    _direction.setSizeLimitForNChars(24, gui::Control::Limit::UseAsMin);
    _flightDate.setSizeLimitForNChars(24, gui::Control::Limit::UseAsMin);
    _origin.setSizeLimitForNChars(36, gui::Control::Limit::UseAsMin);
    _destination.setSizeLimitForNChars(36, gui::Control::Limit::UseAsMin);
    _fareEUR.setSizeLimitForNChars(20, gui::Control::Limit::UseAsMin);
    _extraCostEUR.setSizeLimitForNChars(20, gui::Control::Limit::UseAsMin);

    _fareEUR.setMinValue(0.0);
    _extraCostEUR.setMinValue(0.0);

    td::Date flightDate(true);
    _flightDate.setValue(flightDate);

    _flightList.setAsReadOnly();
    _flightList.setFontSize(10.0f);
    _flightList.setSizeLimits(1350, gui::Control::Limit::UseAsMin,
                              180, gui::Control::Limit::UseAsMin);
    _flightList.setText(tr("initialFlightList"));
    _flightToRemove.setSizeLimitForNChars(70, gui::Control::Limit::UseAsMin);
    updateFlightSelector();
    setBackgroundColor(td::ColorID::SysBackAlt1);
    _removeBar.setBackgroundColor(td::ColorID::SysHeaderBack);
    _removeBarLayout.setMargins(8, 4);
    _removeBarLayout.setSpaceBetweenCells(8);
    _removeBarLayout.append(_flightToRemove, td::HAlignment::Left);
    _removeBarLayout.appendSpacer();
    _removeBarLayout.append(_removeFlightButton, td::HAlignment::Right);
    _removeBar.setLayout(&_removeBarLayout);

    _flightHeader.disableRemeasuring();
    _directionLabel.disableRemeasuring();
    _direction.disableRemeasuring();
    _flightDateLabel.disableRemeasuring();
    _originLabel.disableRemeasuring();
    _destinationLabel.disableRemeasuring();
    _fareLabel.disableRemeasuring();
    _extraCostLabel.disableRemeasuring();
    _addFlightButton.disableRemeasuring();
    _showFlightsButton.disableRemeasuring();
    _status.disableRemeasuring();
    _flightListHeader.disableRemeasuring();
    _flightToRemoveLabel.disableRemeasuring();
    _flightToRemove.disableRemeasuring();
    _flightList.disableRemeasuring();
    _removeFlightButton.disableRemeasuring();

    _layout.setMargins(12, 12);
    _layout.setSpaceBetweenCells(6, 10);
    _layout.insert(0, 0, _flightHeader, 4);
    _layout.insert(1, 0, _directionLabel);
    _layout.insert(1, 1, _direction);
    _layout.insert(1, 2, _flightDateLabel);
    _layout.insert(1, 3, _flightDate);
    _layout.insert(2, 0, _originLabel);
    _layout.insert(2, 1, _origin);
    _layout.insert(2, 2, _destinationLabel);
    _layout.insert(2, 3, _destination);
    _layout.insert(3, 0, _fareLabel);
    _layout.insert(3, 1, _fareEUR);
    _layout.insert(3, 2, _extraCostLabel);
    _layout.insert(3, 3, _extraCostEUR);
    _layout.insert(4, 0, _addFlightButton, 2);
    _layout.insert(4, 2, _showFlightsButton, 2);
    _layout.insert(5, 0, _status, 4);
    _layout.insert(6, 0, _flightListHeader, 4);
    _layout.insert(7, 0, _flightList, 4);
    _layout.insert(8, 0, _flightToRemoveLabel, 4);
    _layout.insert(9, 0, _removeBar, 4);
    setLayout(&_layout);
}

void FlightEntryPage::applyLanguage()
{
    _flightHeader.setTitle(tr("newFlight"));
    _directionLabel.setTitle(tr("flightType"));
    _flightDateLabel.setTitle(tr("flightDate"));
    _originLabel.setTitle(tr("departureLocation"));
    _destinationLabel.setTitle(tr("arrivalLocation"));
    _fareLabel.setTitle(tr("ticketPrice"));
    _extraCostLabel.setTitle(tr("extraCost"));
    _addFlightButton.setTitle(tr("addFlight"));
    _showFlightsButton.setTitle(tr("showFlights"));
    _flightListHeader.setTitle(tr("enteredFlights"));
    _flightToRemoveLabel.setTitle(tr("flightToRemoveLabel"));
    _removeFlightButton.setTitle(tr("removeSelectedFlight"));

    const int selectedDirection = _direction.getSelectedIndex();
    _direction.clean();
    _direction.addItem(tr("departure"));
    _direction.addItem(tr("return"));
    _direction.selectIndex(selectedDirection);

    if (_owner->_flights.empty())
        _flightList.setText(tr("initialFlightList"));
    else
        showFlightOverview();

    updateFlightSelector();
    _status.setTitle(tr("languageChanged"));
    setLayout(&_layout);
}

bool FlightEntryPage::addFlight()
{
    const std::string origin = trim(asStdString(_origin.getText()));
    const std::string destination = trim(asStdString(_destination.getText()));
    const td::Date date = _flightDate.getValue();
    const double fareEUR = _fareEUR.getValue().toNumber<double>();
    const double extraCostEUR = _extraCostEUR.getValue().toNumber<double>();

    if (origin.empty() || destination.empty())
    {
        _status.setTitle(tr("validationLocations"));
        return false;
    }
    if (!isDateSet(date))
    {
        _status.setTitle(tr("validationDate"));
        return false;
    }
    if (!std::isfinite(fareEUR) || fareEUR <= 0.0)
    {
        _status.setTitle(tr("validationFare"));
        return false;
    }
    if (!std::isfinite(extraCostEUR) || extraCostEUR < 0.0)
    {
        _status.setTitle(tr("validationExtraCost"));
        return false;
    }

    const TipLeta tip = _direction.getSelectedIndex() == 0
        ? TipLeta::Polazak : TipLeta::Povratak;
    _owner->_flights.emplace_back(origin, destination, date, fareEUR, extraCostEUR, tip);
    _owner->invalidateRankedTrips();
    showFlightOverview();
    _status.setTitle(tr("statusFlightAdded"));
    return true;
}

void FlightEntryPage::updateFlightSelector()
{
    _flightToRemove.clean();
    _flightToRemove.addItem(tr("selectFlightPlaceholder"));
    for (std::size_t i = 0; i < _owner->_flights.size(); ++i)
    {
        const Let& flight = _owner->_flights[i];
        std::ostringstream item;
        item << i + 1 << ") " << formatDate(flight.getDatum()) << "  •  "
             << flight.getLokacijaPolijetanja() << " -> "
             << flight.getLokacijaSlijetanja();
        _flightToRemove.addItem(item.str().c_str());
    }
    _flightToRemove.selectIndex(0, false);
}

void FlightEntryPage::showFlightOverview()
{
    updateFlightSelector();
    if (_owner->_flights.empty())
    {
        _flightList.setText(tr("noFlights"));
        return;
    }

    std::ostringstream output;
    for (std::size_t i = 0; i < _owner->_flights.size(); ++i)
    {
        const Let& flight = _owner->_flights[i];
        const td::String direction = flight.getTip() == TipLeta::Polazak
            ? tr("departureUpper") : tr("returnUpper");
        output << i + 1 << ") " << direction << "  •  "
               << formatDate(flight.getDatum()) << "  •  "
               << flight.getLokacijaPolijetanja() << " -> "
               << flight.getLokacijaSlijetanja() << "\n    "
               << tr("ticketShort") << " "
               << formatMoney(flight.getCijenaEUR()) << " EUR";
        if (flight.imaDodatneTroskove())
            output << "  +  " << tr("additionalShort") << " "
                   << formatMoney(flight.getDodatniTrosakEUR()) << " EUR";
        else
            output << "  •  " << tr("noAdditionalCost");
        output << "  =  " << tr("totalShort") << " "
               << formatMoney(flight.getUkupnaCijenaEUR()) << " EUR\n";

        if (i + 1 < _owner->_flights.size())
            output << "    ____________________________________________________________\n";
    }
    _flightList.setText(output.str().c_str());
}

void FlightEntryPage::removeSelectedFlight()
{
    const int selectedOption = _flightToRemove.getSelectedIndex();
    if (selectedOption <= 0 ||
        static_cast<std::size_t>(selectedOption - 1) >= _owner->_flights.size())
    {
        _status.setTitle(tr("selectFlightToRemove"));
        return;
    }

    const std::size_t selectedIndex = static_cast<std::size_t>(selectedOption - 1);
    _owner->_flights.erase(_owner->_flights.begin() + selectedIndex);
    _owner->invalidateRankedTrips();
    showFlightOverview();
    _status.setTitle(tr("statusFlightRemoved"));
}

void FlightEntryPage::saveFlights()
{
    const std::filesystem::path filePath = getFlightDataFilePath();
    std::error_code fileError;
    std::filesystem::create_directories(filePath.parent_path(), fileError);
    if (fileError)
    {
        _status.setTitle(tr("statusSaveFailed"));
        return;
    }

    std::ofstream output(filePath, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!output)
    {
        _status.setTitle(tr("statusSaveFailed"));
        return;
    }

    output.imbue(std::locale::classic());
    output << std::fixed << std::setprecision(2);
    for (const Let& flight : _owner->_flights)
    {
        char dateText[11];
        flight.getDatum().formc_str(dateText);
        const char direction = flight.getTip() == TipLeta::Polazak ? 'D' : 'R';
        output << direction << ' ' << dateText << ' '
               << std::quoted(flight.getLokacijaPolijetanja()) << ' '
               << std::quoted(flight.getLokacijaSlijetanja()) << ' '
               << flight.getCijenaEUR() << ' '
               << flight.getDodatniTrosakEUR() << '\n';
    }

    _status.setTitle(output ? tr("statusFlightsSaved") : tr("statusSaveFailed"));
}

void FlightEntryPage::loadFlights()
{
    std::filesystem::path filePath = getFlightDataFilePath();
    std::error_code pathError;
    if (!std::filesystem::exists(filePath, pathError) && !pathError)
    {
        const std::filesystem::path legacyPath(LegacyFlightDataFilePath);
        std::error_code legacyError;
        if (std::filesystem::exists(legacyPath, legacyError) && !legacyError)
            filePath = legacyPath;
    }

    std::ifstream input(filePath, std::ios::in | std::ios::binary);
    if (!input)
    {
        _status.setTitle(tr("statusLoadFailed"));
        return;
    }

    std::vector<Let> loadedFlights;
    std::string line;
    while (std::getline(input, line))
    {
        if (line.empty())
            continue;

        std::istringstream record(line);
        record.imbue(std::locale::classic());
        char direction = '\0';
        std::string dateText, origin, destination;
        double fareEUR = 0.0, extraCostEUR = 0.0;
        if (!(record >> direction >> dateText >> std::quoted(origin)
                     >> std::quoted(destination) >> fareEUR >> extraCostEUR))
        {
            _status.setTitle(tr("statusLoadFailed"));
            return;
        }

        record >> std::ws;
        if (!record.eof() || (direction != 'D' && direction != 'R') ||
            origin.empty() || destination.empty() ||
            !std::isfinite(fareEUR) || fareEUR <= 0.0 ||
            !std::isfinite(extraCostEUR) || extraCostEUR < 0.0)
        {
            _status.setTitle(tr("statusLoadFailed"));
            return;
        }

        td::Date date;
        date.fromString(dateText.c_str());
        char canonicalDate[11];
        date.formc_str(canonicalDate);
        if (!isDateSet(date) || dateText != canonicalDate)
        {
            _status.setTitle(tr("statusLoadFailed"));
            return;
        }

        const TipLeta tip = direction == 'D' ? TipLeta::Polazak : TipLeta::Povratak;
        loadedFlights.emplace_back(origin, destination, date, fareEUR, extraCostEUR, tip);
    }

    if (input.bad())
    {
        _status.setTitle(tr("statusLoadFailed"));
        return;
    }

    _owner->_flights.insert(_owner->_flights.end(), loadedFlights.begin(), loadedFlights.end());
    _owner->invalidateRankedTrips();
    showFlightOverview();
    _status.setTitle(tr("statusFlightsLoaded"));
}

bool FlightEntryPage::onClick(gui::Button* pButton)
{
    if (pButton == &_addFlightButton)
    {
        addFlight();
        return true;
    }
    if (pButton == &_showFlightsButton)
    {
        showFlightOverview();
        return true;
    }
    if (pButton == &_removeFlightButton)
        return true;
    return false;
}

TripPlannerPage::TripPlannerPage(FlightPlannerView* owner)
    : _owner(owner)
    , _filterHeader(tr("filtersRanking"))
    , _meetingCityLabel(tr("meetingCity"))
    , _earliestDateLabel(tr("earliestDeparture"))
    , _latestDateLabel(tr("latestReturn"))
    , _minimumNightsLabel(tr("minimumNights"))
    , _minimumNights(td::int4)
    , _maximumNightsLabel(tr("maximumNights"))
    , _maximumNights(td::int4)
    , _maximumBudgetLabel(tr("maximumBudget"))
    , _maximumBudgetEUR(td::decimal2)
    , _sortOrderLabel(tr("sortBy"))
    , _rankButton(tr("rankTrips"))
    , _resultsHeader(tr("possibleTrips"))
    , _previousPageButton(tr("previous"))
    , _pageLabel("0 / 0")
    , _nextPageButton(tr("next"))
    , _paginationLayout(4)
    , _layout(10, 4)
    , _currentPage(0)
{
    _rankButton.setType(gui::Button::Type::Default);
    _sortOrder.addItem(tr("lowestTotalCost"));
    _sortOrder.addItem(tr("longestStay"));
    _sortOrder.addItem(tr("shortestStay"));
    _sortOrder.selectIndex(0);

    _meetingCityLabel.setSizeLimitForNChars(18, gui::Control::Limit::UseAsMin);
    _earliestDateLabel.setSizeLimitForNChars(18, gui::Control::Limit::UseAsMin);
    _latestDateLabel.setSizeLimitForNChars(18, gui::Control::Limit::UseAsMin);
    _minimumNightsLabel.setSizeLimitForNChars(18, gui::Control::Limit::UseAsMin);
    _maximumNightsLabel.setSizeLimitForNChars(18, gui::Control::Limit::UseAsMin);
    _maximumBudgetLabel.setSizeLimitForNChars(18, gui::Control::Limit::UseAsMin);
    _sortOrderLabel.setSizeLimitForNChars(18, gui::Control::Limit::UseAsMin);
    _meetingCity.setSizeLimitForNChars(36, gui::Control::Limit::UseAsMin);
    _earliestDate.setSizeLimitForNChars(22, gui::Control::Limit::UseAsMin);
    _latestDate.setSizeLimitForNChars(22, gui::Control::Limit::UseAsMin);
    _minimumNights.setSizeLimitForNChars(12, gui::Control::Limit::UseAsMin);
    _maximumNights.setSizeLimitForNChars(12, gui::Control::Limit::UseAsMin);
    _maximumBudgetEUR.setSizeLimitForNChars(20, gui::Control::Limit::UseAsMin);
    _sortOrder.setSizeLimitForNChars(24, gui::Control::Limit::UseAsMin);

    _minimumNights.setMinValue(0.0);
    _minimumNights.setMaxValue(3650.0);
    _maximumNights.setMinValue(0.0);
    _maximumNights.setMaxValue(3650.0);
    _maximumBudgetEUR.setMinValue(0.0);
    _minimumNights.setValue(0);
    _maximumNights.setValue(365);
    _maximumBudgetEUR.setValue(td::Decimal2(0.0));

    td::Date earliestDeparture(true);
    _earliestDate.setValue(earliestDeparture);
    _latestDate.setValue(earliestDeparture + 365);
    _results.setAsReadOnly();
    _results.setFontSize(10.0f);
    _results.setSizeLimits(1350, gui::Control::Limit::UseAsMin,
                           260, gui::Control::Limit::UseAsMin);
    _results.setText(tr("initialResults"));
    setBackgroundColor(td::ColorID::SysBackAlt1);
    _paginationBar.setBackgroundColor(td::ColorID::SysHeaderBack);
    _paginationLayout.setMargins(8, 4);
    _paginationLayout.setSpaceBetweenCells(8);
    _paginationLayout.append(_previousPageButton, td::HAlignment::Left);
    _paginationLayout.append(_nextPageButton, td::HAlignment::Left);
    _paginationLayout.appendSpace(12);
    _paginationLayout.append(_pageLabel, td::HAlignment::Left);
    _paginationBar.setLayout(&_paginationLayout);

    _previousPageButton.onClick([this]
    {
        if (_currentPage > 0)
        {
            --_currentPage;
            showResults();
        }
    });
    _nextPageButton.onClick([this]
    {
        const std::size_t totalPages =
            (_owner->_rankedOptions.size() + ResultsPerPage - 1) / ResultsPerPage;
        if (_currentPage + 1 < totalPages)
        {
            ++_currentPage;
            showResults();
        }
    });

    _filterHeader.disableRemeasuring();
    _meetingCityLabel.disableRemeasuring();
    _earliestDateLabel.disableRemeasuring();
    _latestDateLabel.disableRemeasuring();
    _minimumNightsLabel.disableRemeasuring();
    _maximumNightsLabel.disableRemeasuring();
    _maximumBudgetLabel.disableRemeasuring();
    _sortOrderLabel.disableRemeasuring();
    _sortOrder.disableRemeasuring();
    _rankButton.disableRemeasuring();
    _resultsHeader.disableRemeasuring();
    _results.disableRemeasuring();
    _previousPageButton.disableRemeasuring();
    _pageLabel.disableRemeasuring();
    _nextPageButton.disableRemeasuring();
    _status.disableRemeasuring();

    _layout.setMargins(12, 12);
    _layout.setSpaceBetweenCells(6, 10);
    _layout.insert(0, 0, _filterHeader, 4);
    _layout.insert(1, 0, _meetingCityLabel);
    _layout.insert(1, 1, _meetingCity, 3);
    _layout.insert(2, 0, _earliestDateLabel);
    _layout.insert(2, 1, _earliestDate);
    _layout.insert(2, 2, _latestDateLabel);
    _layout.insert(2, 3, _latestDate);
    _layout.insert(3, 0, _minimumNightsLabel);
    _layout.insert(3, 1, _minimumNights);
    _layout.insert(3, 2, _maximumNightsLabel);
    _layout.insert(3, 3, _maximumNights);
    _layout.insert(4, 0, _maximumBudgetLabel);
    _layout.insert(4, 1, _maximumBudgetEUR);
    _layout.insert(4, 2, _sortOrderLabel);
    _layout.insert(4, 3, _sortOrder);
    _layout.insert(5, 0, _rankButton, 4);
    _layout.insert(6, 0, _resultsHeader, 4);
    _layout.insert(7, 0, _results, 4);
    _layout.insert(8, 0, _paginationBar, 4);
    _layout.insert(9, 0, _status, 4);
    setLayout(&_layout);
}

void TripPlannerPage::applyLanguage()
{
    _filterHeader.setTitle(tr("filtersRanking"));
    _meetingCityLabel.setTitle(tr("meetingCity"));
    _earliestDateLabel.setTitle(tr("earliestDeparture"));
    _latestDateLabel.setTitle(tr("latestReturn"));
    _minimumNightsLabel.setTitle(tr("minimumNights"));
    _maximumNightsLabel.setTitle(tr("maximumNights"));
    _maximumBudgetLabel.setTitle(tr("maximumBudget"));
    _sortOrderLabel.setTitle(tr("sortBy"));
    _rankButton.setTitle(tr("rankTrips"));
    _resultsHeader.setTitle(tr("possibleTrips"));
    _previousPageButton.setTitle(tr("previous"));
    _nextPageButton.setTitle(tr("next"));

    const int selectedSortOrder = _sortOrder.getSelectedIndex();
    _sortOrder.clean();
    _sortOrder.addItem(tr("lowestTotalCost"));
    _sortOrder.addItem(tr("longestStay"));
    _sortOrder.addItem(tr("shortestStay"));
    _sortOrder.selectIndex(selectedSortOrder);

    if (_owner->_hasRanked)
        showResults();
    else
        clearResults();
    _status.setTitle(tr("languageChanged"));
    setLayout(&_layout);
}

bool TripPlannerPage::readFilter(FilterPutovanja& filter,
                                 std::string& errorMessage) const
{
    filter.gradSusreta = trim(asStdString(_meetingCity.getText()));
    filter.najranijiPolazak = _earliestDate.getValue();
    filter.koristiNajranijiPolazak = isDateSet(filter.najranijiPolazak);
    filter.najkasnijiPovratak = _latestDate.getValue();
    filter.koristiNajkasnijiPovratak = isDateSet(filter.najkasnijiPovratak);
    filter.minimalanBrojNocenja = _minimumNights.getValue().i4Val();
    filter.maksimalanBrojNocenja = _maximumNights.getValue().i4Val();

    if (filter.minimalanBrojNocenja > filter.maksimalanBrojNocenja)
    {
        errorMessage = asStdString(tr("validationNightRange"));
        return false;
    }

    filter.maksimalnaCijenaEUR =
        _maximumBudgetEUR.getValue().toNumber<double>();
    filter.koristiMaksimalnuCijenu = filter.maksimalnaCijenaEUR > 0.0;

    if (_sortOrder.getSelectedIndex() == 1)
        filter.nacinRangiranja = NacinRangiranja::NajduziBoravak;
    else if (_sortOrder.getSelectedIndex() == 2)
        filter.nacinRangiranja = NacinRangiranja::NajkraciBoravak;
    else
        filter.nacinRangiranja = NacinRangiranja::NajnizaCijena;

    if (filter.koristiNajranijiPolazak && filter.koristiNajkasnijiPovratak &&
        filter.najkasnijiPovratak < filter.najranijiPolazak)
    {
        errorMessage = asStdString(tr("validationDateRange"));
        return false;
    }
    return true;
}

void TripPlannerPage::rankFlights()
{
    FilterPutovanja filter;
    std::string errorMessage;
    if (!readFilter(filter, errorMessage))
    {
        _status.setTitle(errorMessage.c_str());
        return;
    }

    _owner->_rankedOptions = RangiracLetova::rangiraj(_owner->_flights, filter);
    _currentPage = 0;
    _owner->_hasRanked = true;
    showResults();
}

void TripPlannerPage::clearResults()
{
    _results.setText(tr("initialResults"));
    _pageLabel.setTitle("0 / 0");
    _status.setTitle("");
}

void TripPlannerPage::showResults()
{
    if (_owner->_rankedOptions.empty())
    {
        _results.setText(tr("noMatchingTrips"));
        _pageLabel.setTitle("0 / 0");
        _status.setTitle(tr("statusNoTrips"));
        return;
    }

    const std::size_t totalPages =
        (_owner->_rankedOptions.size() + ResultsPerPage - 1) / ResultsPerPage;
    if (_currentPage >= totalPages)
        _currentPage = totalPages - 1;
    const std::size_t startIndex = _currentPage * ResultsPerPage;
    const std::size_t endIndex =
        std::min(startIndex + ResultsPerPage, _owner->_rankedOptions.size());

    std::ostringstream output;
    for (std::size_t i = startIndex; i < endIndex; ++i)
    {
        const OpcijaPutovanja& option = _owner->_rankedOptions[i];
        const Let& departure = _owner->_flights[option.indeksPolaska];
        const Let& returnFlight = _owner->_flights[option.indeksPovratka];
        const double fareTotal =
            departure.getCijenaEUR() + returnFlight.getCijenaEUR();
        const double extraTotalEUR =
            departure.getDodatniTrosakEUR() + returnFlight.getDodatniTrosakEUR();

        output << i + 1 << ") " << formatDate(departure.getDatum()) << " - "
               << formatDate(returnFlight.getDatum()) << "  •  "
               << option.brojNocenja << " " << tr("nightsUnit") << "  •  "
               << departure.getLokacijaPolijetanja() << " -> "
               << departure.getLokacijaSlijetanja() << " / "
               << returnFlight.getLokacijaPolijetanja() << " -> "
               << returnFlight.getLokacijaSlijetanja() << "\n    "
               << tr("ticketsShort") << " " << formatMoney(fareTotal)
               << " EUR  +  " << tr("extraCostsShort") << " "
               << formatMoney(extraTotalEUR) << " EUR  =  "
               << tr("totalShort") << " "
               << formatMoney(option.ukupnaCijenaEUR) << " EUR\n";

        if (i + 1 < endIndex)
            output << "    ________________________________________________________________________________________________\n";
    }
    _results.setText(output.str().c_str());

    std::ostringstream pageText;
    pageText << (_currentPage + 1) << " / " << totalPages << "  ("
             << _owner->_rankedOptions.size() << " " << tr("combinationsUnit") << ")";
    _pageLabel.setTitle(pageText.str().c_str());

    std::ostringstream statusText;
    statusText << tr("foundCombinations") << _owner->_rankedOptions.size()
               << ". " << tr("totalIncludesAdditionalFees");
    _status.setTitle(statusText.str().c_str());
}

void TripPlannerPage::setDefaultSortOrder(int sortIndex)
{
    if (sortIndex >= 0 && sortIndex <= 2)
        _sortOrder.selectIndex(sortIndex, false);
}

bool TripPlannerPage::onClick(gui::Button* pButton)
{
    if (pButton == &_rankButton)
    {
        rankFlights();
        return true;
    }
    if (pButton == &_previousPageButton)
        return true;
    if (pButton == &_nextPageButton)
        return true;
    return false;
}

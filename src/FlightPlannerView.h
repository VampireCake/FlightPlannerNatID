#pragma once

#include "Let.h"
#include "TripRanker.h"

#include <gui/View.h>
#include <gui/StandardTabView.h>
#include <gui/Label.h>
#include <gui/LineEdit.h>
#include <gui/NumericEdit.h>
#include <gui/DateEdit.h>
#include <gui/ComboBox.h>
#include <gui/Button.h>
#include <gui/TextEdit.h>
#include <gui/GridLayout.h>
#include <gui/HorizontalLayout.h>
#include <td/String.h>

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class FlightPlannerView;

class FlightEntryPage : public gui::View
{
    friend class FlightPlannerView;

private:
    FlightPlannerView* _owner;
    gui::Label _flightHeader;
    gui::Label _directionLabel;
    gui::ComboBox _direction;
    gui::Label _flightDateLabel;
    gui::DateEdit _flightDate;
    gui::Label _originLabel;
    gui::LineEdit _origin;
    gui::Label _destinationLabel;
    gui::LineEdit _destination;
    gui::Label _fareLabel;
    gui::NumericEdit _fareEUR;
    gui::Label _extraCostLabel;
    gui::NumericEdit _extraCostEUR;
    gui::Button _addFlightButton;
    gui::Button _showFlightsButton;
    gui::Label _status;
    gui::Label _flightListHeader;
    gui::Label _flightToRemoveLabel;
    gui::ComboBox _flightToRemove;
    gui::TextEdit _flightList;
    gui::Button _removeFlightButton;
    gui::HorizontalLayout _removeBarLayout;
    gui::View _removeBar;
    gui::GridLayout _layout;

    bool addFlight();
    void updateFlightSelector();
    void showFlightOverview();
    void removeSelectedFlight();
    void saveFlights();
    void loadFlights();
    void applyLanguage();
    bool onClick(gui::Button* pButton) override;

protected:
    bool shouldClose() override { return false; }

public:
    explicit FlightEntryPage(FlightPlannerView* owner);
};

class TripPlannerPage : public gui::View
{
    friend class FlightPlannerView;

private:
    FlightPlannerView* _owner;
    gui::Label _filterHeader;
    gui::Label _meetingCityLabel;
    gui::LineEdit _meetingCity;
    gui::Label _earliestDateLabel;
    gui::DateEdit _earliestDate;
    gui::Label _latestDateLabel;
    gui::DateEdit _latestDate;
    gui::Label _minimumNightsLabel;
    gui::NumericEdit _minimumNights;
    gui::Label _maximumNightsLabel;
    gui::NumericEdit _maximumNights;
    gui::Label _maximumBudgetLabel;
    gui::NumericEdit _maximumBudgetEUR;
    gui::Label _sortOrderLabel;
    gui::ComboBox _sortOrder;
    gui::Button _rankButton;
    gui::Label _resultsHeader;
    gui::TextEdit _results;
    gui::Button _previousPageButton;
    gui::Label _pageLabel;
    gui::Button _nextPageButton;
    gui::Label _status;
    gui::HorizontalLayout _paginationLayout;
    gui::View _paginationBar;
    gui::GridLayout _layout;
    std::size_t _currentPage;

    enum { ResultsPerPage = 12 };

    bool readFilter(FilterPutovanja& filter, std::string& errorMessage) const;
    void rankFlights();
    void showResults();
    void clearResults();
    void setDefaultSortOrder(int sortIndex);
    void applyLanguage();
    bool onClick(gui::Button* pButton) override;

protected:
    bool shouldClose() override { return false; }

public:
    explicit TripPlannerPage(FlightPlannerView* owner);
};

class FlightPlannerView : public gui::View
{
    friend class FlightEntryPage;
    friend class TripPlannerPage;

protected:
    void systemColorModeChanged(bool bDarkMode) override;

private:
    std::unique_ptr<FlightEntryPage> _flightPage;
    std::unique_ptr<TripPlannerPage> _tripPage;
    gui::StandardTabView _tabs;
    gui::GridLayout _layout;
    std::vector<Let> _flights;
    std::vector<OpcijaPutovanja> _rankedOptions;
    bool _hasRanked;

    void invalidateRankedTrips();

public:
    FlightPlannerView();
    void showPage(int pageIndex);
    void applyLanguage();
    void changeLanguage(const td::String& extension);
    void setDefaultSortOrder(int sortIndex);
    void saveFlights();
    void loadFlights();
};

#pragma once

#include <gui/MenuBar.h>

class FlightPlannerMenuBar : public gui::MenuBar
{
public:
    enum : td::BYTE
    {
        FlightsMenuID = 10,
        CalendarMenuID = 20,
        MapMenuID = 30,
        ExportMenuID = 40,
        TripsMenuID = 50,
        LanguageMenuID = 60,
        SettingsMenuID = 70,
        HelpMenuID = 80,
        DefaultRankingMenuID = 1,
        LoadFlightsID = 1,
        SaveFlightsID = 2,
        AboutActionID = 1,
        QuickGuideActionID = 2,
        DefaultSortByCostID = 1,
        DefaultSortLongestStayID = 2,
        DefaultSortShortestStayID = 3,
        FirstLanguageActionID = 10
    };

private:
    gui::SubMenu _flightsMenu;
    gui::SubMenu _calendarMenu;
    gui::SubMenu _mapMenu;
    gui::SubMenu _exportMenu;
    gui::SubMenu _tripsMenu;
    gui::SubMenu _settingsMenu;
    gui::SubMenu _helpMenu;

public:
    FlightPlannerMenuBar()
        : gui::MenuBar(7)
        , _flightsMenu(FlightsMenuID, tr("menuFlights"), 2)
        , _calendarMenu(CalendarMenuID, tr("menuCalendar"), 1)
        , _mapMenu(MapMenuID, tr("menuMap"), 1)
        , _exportMenu(ExportMenuID, tr("menuExport"), 2)
        , _tripsMenu(TripsMenuID, tr("menuTrips"), 3)
        , _settingsMenu(SettingsMenuID, tr("menuSettings"), 2)
        , _helpMenu(HelpMenuID, tr("menuHelp"), 2)
    {
        auto& flightItems = _flightsMenu.getItems();
        flightItems[0].initAsActionItem(tr("menuLoadFlights"), LoadFlightsID);
        flightItems[1].initAsActionItem(tr("menuSaveFlights"), SaveFlightsID);

        _calendarMenu.getItems()[0].initAsActionItem(tr("menuCalendarOpen"), 1);
        _mapMenu.getItems()[0].initAsActionItem(tr("menuMapOpen"), 1);

        auto& exportItems = _exportMenu.getItems();
        exportItems[0].initAsActionItem(tr("menuExportCsv"), 1);
        exportItems[1].initAsActionItem(tr("menuExportPdf"), 2);

        auto& tripItems = _tripsMenu.getItems();
        tripItems[0].initAsActionItem(tr("menuAllTrips"), 1);
        tripItems[1].initAsActionItem(tr("menuSavedTrips"), 2);
        tripItems[2].initAsActionItem(tr("menuFavoriteTrips"), 3);

        auto& settingItems = _settingsMenu.getItems();
        settingItems[0].initAsSubMenu(DefaultRankingMenuID, tr("menuDefaultRanking"), 3);
        settingItems[1].initAsSubMenu(LanguageMenuID, tr("menuLanguage"), 5);

        auto& rankingItems = settingItems[0].getItems();
        rankingItems[0].initAsActionItem(tr("menuDefaultSortCost"), DefaultSortByCostID);
        rankingItems[1].initAsActionItem(tr("menuDefaultSortLongest"), DefaultSortLongestStayID);
        rankingItems[2].initAsActionItem(tr("menuDefaultSortShortest"), DefaultSortShortestStayID);
        for (size_t i = 0; i < 3; ++i)
            rankingItems[i].setAsCheckable(i == 0);

        const auto& languages = getSupportedLanguages();
        auto& languageItems = settingItems[1].getItems();
        const int currentLanguageIndex = getTranslationLanguageIndex();
        for (size_t i = 0; i < languages.size() && i < languageItems.size(); ++i)
        {
            const td::BYTE actionID = static_cast<td::BYTE>(FirstLanguageActionID + i);
            languageItems[i].initAsActionItem(languages[i].getDescription(), actionID);
            languageItems[i].setAsCheckable(static_cast<int>(i) == currentLanguageIndex);
        }

        auto& helpItems = _helpMenu.getItems();
        helpItems[0].initAsActionItem(tr("menuQuickGuide"), QuickGuideActionID);
        helpItems[1].initAsActionItem(tr("menuAbout"), AboutActionID);

        _menus[0] = &_flightsMenu;
        _menus[1] = &_calendarMenu;
        _menus[2] = &_mapMenu;
        _menus[3] = &_exportMenu;
        _menus[4] = &_tripsMenu;
        _menus[5] = &_settingsMenu;
        _menus[6] = &_helpMenu;
    }

    void setSelectedLanguage(size_t languageIndex)
    {
        auto& languageItems = _settingsMenu.getItems()[1].getItems();
        for (size_t i = 0; i < languageItems.size(); ++i)
            languageItems[i].setChecked(i == languageIndex);
    }

    void setDefaultSortOrder(size_t sortIndex)
    {
        auto& settingItems = _settingsMenu.getItems()[0].getItems();
        for (size_t i = 0; i < 3; ++i)
            settingItems[i].setChecked(i == sortIndex);
    }

    size_t getLanguageIndex(const gui::ActionItem* selectedItem)
    {
        auto& languageItems = _settingsMenu.getItems()[1].getItems();
        for (size_t i = 0; i < languageItems.size(); ++i)
        {
            if (selectedItem == &languageItems[i])
                return i;
        }
        return languageItems.size();
    }
};

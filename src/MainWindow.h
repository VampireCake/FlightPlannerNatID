#pragma once

#include <gui/Application.h>
#include <gui/Alert.h>
#include <gui/Window.h>
#include "FlightPlannerView.h"
#include "MenuBar.h"

class MainWindow : public gui::Window
{
private:
    FlightPlannerMenuBar _menuBar;
    FlightPlannerView _mainView;

protected:
    void systemColorModeChanged(bool bDarkMode) override
    {
        gui::Window::systemColorModeChanged(bDarkMode);
        setBackgroundColor(td::ColorID::SysHeaderBack);
    }

public:
    MainWindow()
        : gui::Window(gui::Size(1350, 900))
    {
        setTitle(tr("appTitle"));
        setBackgroundColor(td::ColorID::SysHeaderBack);
        _menuBar.setAsMain(this, gui::MenuBar::Location::EmbeddedInWindow);
        setCentralView(&_mainView);

        int savedSortOrder = 0;
        if (auto* properties = getAppProperties())
            savedSortOrder = properties->getValue("defaultTripSort", 0);
        if (savedSortOrder < 0 || savedSortOrder > 2)
            savedSortOrder = 0;
        _mainView.setDefaultSortOrder(savedSortOrder);
        _menuBar.setDefaultSortOrder(static_cast<size_t>(savedSortOrder));
    }

    bool onActionItem(gui::ActionItemDescriptor& action) override
    {
        const auto ids = action.getIDs();
        const td::BYTE menuID = std::get<0>(ids);

        if (menuID == FlightPlannerMenuBar::FlightsMenuID)
        {
            const td::BYTE actionID = std::get<3>(ids);
            if (actionID == FlightPlannerMenuBar::LoadFlightsID)
                _mainView.loadFlights();
            else if (actionID == FlightPlannerMenuBar::SaveFlightsID)
                _mainView.saveFlights();
            return true;
        }

        if (menuID == FlightPlannerMenuBar::CalendarMenuID ||
            menuID == FlightPlannerMenuBar::MapMenuID ||
            menuID == FlightPlannerMenuBar::ExportMenuID ||
            menuID == FlightPlannerMenuBar::TripsMenuID)
        {
            // These menu commands are placeholders for future features.
            return true;
        }

        if (menuID == FlightPlannerMenuBar::SettingsMenuID)
        {
            const size_t languageIndex =
                _menuBar.getLanguageIndex(action.getActionItem());
            const auto& languages = getSupportedLanguages();
            if (languageIndex < languages.size())
            {
                td::String extension = languages[languageIndex].getExtension();
                extension.toLower();
                _mainView.changeLanguage(extension);
                setTitle(tr("appTitle"));
                _menuBar.setSelectedLanguage(languageIndex);
                return true;
            }

            const td::BYTE actionID = std::get<3>(ids);
            int sortIndex = -1;
            if (actionID == FlightPlannerMenuBar::DefaultSortByCostID)
                sortIndex = 0;
            else if (actionID == FlightPlannerMenuBar::DefaultSortLongestStayID)
                sortIndex = 1;
            else if (actionID == FlightPlannerMenuBar::DefaultSortShortestStayID)
                sortIndex = 2;

            if (sortIndex >= 0)
            {
                _mainView.setDefaultSortOrder(sortIndex);
                _menuBar.setDefaultSortOrder(static_cast<size_t>(sortIndex));
                if (auto* properties = getAppProperties())
                {
                    properties->setValue("defaultTripSort", sortIndex);
                    properties->save();
                }
            }
            return true;
        }

        if (menuID == FlightPlannerMenuBar::HelpMenuID)
        {
            const td::BYTE actionID = std::get<3>(ids);
            if (actionID == FlightPlannerMenuBar::AboutActionID)
                gui::Alert::show(tr("aboutTitle"), tr("aboutMessage"));
            else if (actionID == FlightPlannerMenuBar::QuickGuideActionID)
                gui::Alert::show(tr("quickGuideTitle"), tr("quickGuideMessage"));
            return true;
        }

        return false;
    }
};

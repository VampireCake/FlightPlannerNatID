set(APPFLIGHTPLANNER_NAME appFlightPlanner)

file(GLOB APPFLIGHTPLANNER_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB APPFLIGHTPLANNER_INCS ${CMAKE_CURRENT_LIST_DIR}/src/*.h)
file(GLOB APPFLIGHTPLANNER_INC_TD ${NATID_SDK_INC}/td/*.h)
file(GLOB APPFLIGHTPLANNER_INC_GUI ${NATID_SDK_INC}/gui/*.h)
set(APPFLIGHTPLANNER_PLIST ${CMAKE_CURRENT_LIST_DIR}/src/Info.plist)
set(DevResPathLoc ${CMAKE_CURRENT_LIST_DIR})

add_executable(${APPFLIGHTPLANNER_NAME}
    ${APPFLIGHTPLANNER_INCS}
    ${APPFLIGHTPLANNER_SOURCES}
    ${APPFLIGHTPLANNER_INC_TD}
    ${APPFLIGHTPLANNER_INC_GUI})

if (WIN32)
    set_target_properties(${APPFLIGHTPLANNER_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")

    # Embed the application icon so Windows can use it for the taskbar button.
    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/res/appIcon.rc.in"
        "${CMAKE_CURRENT_BINARY_DIR}/appIcon.rc"
        @ONLY)
    target_sources(${APPFLIGHTPLANNER_NAME} PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/appIcon.rc")
endif()

source_group("inc" FILES ${APPFLIGHTPLANNER_INCS})
source_group("inc\\td" FILES ${APPFLIGHTPLANNER_INC_TD})
source_group("inc\\gui" FILES ${APPFLIGHTPLANNER_INC_GUI})
source_group("src" FILES ${APPFLIGHTPLANNER_SOURCES})

target_link_libraries(${APPFLIGHTPLANNER_NAME}
    debug ${MU_LIB_DEBUG}
    debug ${NATGUI_LIB_DEBUG}
    optimized ${MU_LIB_RELEASE}
    optimized ${NATGUI_LIB_RELEASE})

setTargetPropertiesForGUIApp(${APPFLIGHTPLANNER_NAME} ${APPFLIGHTPLANNER_PLIST})
setIDEPropertiesForGUIExecutable(${APPFLIGHTPLANNER_NAME} ${CMAKE_CURRENT_LIST_DIR})
setPlatformDLLPath(${APPFLIGHTPLANNER_NAME})

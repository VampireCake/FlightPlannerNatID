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
option(FLIGHTPLANNER_SKIP_DEVRES_REGISTRATION
    "Avoid writing project paths into the external natID SDK during packaging"
    OFF)

if (WIN32 AND FLIGHTPLANNER_SKIP_DEVRES_REGISTRATION)
    # Keep the Visual Studio launch settings but do not let the SDK rewrite
    # DevEnv/DevResPaths.txt inside its shared installation directory.
    set_target_properties(${APPFLIGHTPLANNER_NAME} PROPERTIES
        VS_DEBUGGER_COMMAND_ARGUMENTS "-devResPath=${CMAKE_CURRENT_LIST_DIR}"
        WIN32_EXECUTABLE TRUE)
else()
    setIDEPropertiesForGUIExecutable(${APPFLIGHTPLANNER_NAME} ${CMAKE_CURRENT_LIST_DIR})
endif()
setPlatformDLLPath(${APPFLIGHTPLANNER_NAME})

if (WIN32)
    install(TARGETS ${APPFLIGHTPLANNER_NAME}
        RUNTIME DESTINATION ".")

    # Include release natID DLLs, GTK plugins/data, shared UI resources,
    # and the Common resource tree referenced by res/DevRes.xml.
    file(GLOB NATID_BIN_DLLS "${NATID_SDK_BIN}/*.dll")
    foreach(NATID_BIN_DLL IN LISTS NATID_BIN_DLLS)
        get_filename_component(NATID_BIN_DLL_NAME "${NATID_BIN_DLL}" NAME)
        if (NOT NATID_BIN_DLL_NAME MATCHES "D\\.dll$")
            install(FILES "${NATID_BIN_DLL}" DESTINATION ".")
        endif()
    endforeach()
    # Put GTK's top-level runtime DLLs next to the executable. Windows resolves
    # transitive DLL imports from the executable directory; relying only on
    # PATH set by the VBS launcher can fail for shortcuts or direct launches.
    file(GLOB GTK_RUNTIME_DLLS "${NATID_SDK_BIN}/GTK/*.dll")
    install(FILES ${GTK_RUNTIME_DLLS} DESTINATION ".")
    install(DIRECTORY "${NATID_SDK_BIN}/GTK/"
        DESTINATION "GTK"
        FILES_MATCHING PATTERN "*.dll")
    install(DIRECTORY "${NATID_SDK_BIN}/GTK/release/share/"
        DESTINATION "GTK/release/share")
    install(DIRECTORY "${NATID_SDK_ROOT}/Common/Regionals/"
        DESTINATION "Common/Regionals")
    install(FILES "${NATID_SDK_ROOT}/Common/ResConfigs/natGUI.xml"
        DESTINATION "Common/ResConfigs")
    install(FILES "${NATID_SDK_ROOT}/Common/Icons/defaultImg.png"
        DESTINATION "Common/Icons")
    install(DIRECTORY "${NATID_SDK_ROOT}/Common/Icons/Msg/"
        DESTINATION "Common/Icons/Msg")
    install(FILES "${NATID_SDK_ROOT}/Common/Symbols/PSA/pointer.xml"
        DESTINATION "Common/Symbols/PSA")
    install(FILES "${NATID_SDK_ROOT}/Common/Symbols/Shapes/defaultSymbol.xml"
        DESTINATION "Common/Symbols/Shapes")
    install(FILES "${NATID_SDK_ROOT}/Common/Sounds/Click.wav"
        DESTINATION "Common/Sounds")
    install(DIRECTORY "${SOURCE_ROOT}/res/"
        DESTINATION "res")
    install(FILES "${SOURCE_ROOT}/installer/FlightPlannerNatID.vbs"
        DESTINATION ".")
endif()

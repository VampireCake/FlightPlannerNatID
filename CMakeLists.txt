cmake_minimum_required(VERSION 3.18)

set(SOLUTION_NAME FlightPlannerNatIDSol)
project(${SOLUTION_NAME} VERSION 0.1.0)

# natID SDK is an external dependency and is not included in this repository.
set(SOURCE_ROOT ${CMAKE_CURRENT_LIST_DIR})
if (WIN32)
    set(NATID_SDK_DEFAULT "$ENV{USERPROFILE}/natID.SDK")
else()
    set(NATID_SDK_DEFAULT "$ENV{HOME}/natID.SDK")
endif()

set(NATID_SDK_ROOT "${NATID_SDK_DEFAULT}" CACHE PATH "Path to the natID SDK")
if (NOT EXISTS "${NATID_SDK_ROOT}/DevEnv/Common.cmake")
    message(FATAL_ERROR
        "natID SDK was not found at '${NATID_SDK_ROOT}'. "
        "Set NATID_SDK_ROOT to the SDK directory when configuring CMake.")
endif()

include(${NATID_SDK_ROOT}/DevEnv/Common.cmake)
include(${NATID_SDK_ROOT}/DevEnv/natGUI.cmake)

# Keep generated binaries in this build tree so the build does not depend on the SDK RAM disk.
set(APP_OUTPUT_ROOT "${CMAKE_CURRENT_BINARY_DIR}/FlightPlannerNatIDSol")
foreach(CONFIG DEBUG RELEASE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${APP_OUTPUT_ROOT}/${CONFIG}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${APP_OUTPUT_ROOT}/${CONFIG}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${APP_OUTPUT_ROOT}/${CONFIG}")
    set(CMAKE_PDB_OUTPUT_DIRECTORY_${CONFIG} "${APP_OUTPUT_ROOT}/${CONFIG}")
    set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY_${CONFIG} "${APP_OUTPUT_ROOT}/${CONFIG}")
endforeach()

include(appFlightPlanner.cmake)

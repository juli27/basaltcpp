cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project("Basalt" VERSION "0.4" LANGUAGES "CXX")

set(BASALT_BUILD_DEVEL ON CACHE BOOL
    "Configure a development or release build")
set(BASALT_BUILD_WARNINGS_AS_ERRORS OFF CACHE BOOL
    "Treat compiler and linker warnings as errors")

include(FeatureSummary)

include(BasaltCommonFlags)

add_subdirectory("external" EXCLUDE_FROM_ALL)
add_subdirectory("libapi")
add_subdirectory("libruntime")
add_subdirectory("platformlibs/libwin32")
add_subdirectory("sandbox")

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Sandbox.Win32)

feature_summary(WHAT ALL)

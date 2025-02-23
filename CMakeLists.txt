cmake_minimum_required(VERSION 3.23 FATAL_ERROR)
project(Basalt VERSION 0.4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD "17" CACHE STRING
    "Default value for CXX_STANDARD property of targets")
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL
    "Default value for CXX_EXTENSIONS property of targets")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON CACHE BOOL
    "Default value for INTERPROCEDURAL_OPTIMIZATION of targets")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF CACHE BOOL
    "Default value for INTERPROCEDURAL_OPTIMIZATION_DEBUG of targets")
set(CMAKE_OPTIMIZE_DEPENDENCIES ON CACHE BOOL
    "Initializes the OPTIMIZE_DEPENDENCIES target property")

set(BASALT_BUILD_DEVEL ON CACHE BOOL
    "Configure a development or release build")
set(BASALT_BUILD_WARNINGS_AS_ERRORS OFF CACHE BOOL
    "Treat compiler and linker warnings as errors")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(Compiler)
include(FeatureSummary)

add_subdirectory("external" EXCLUDE_FROM_ALL)
add_subdirectory("libapi")
add_subdirectory("libruntime")
add_subdirectory("platformlibs/libwin32")
add_subdirectory("sandbox")

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Sandbox.Win32)

feature_summary(WHAT ALL)

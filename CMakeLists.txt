cmake_minimum_required(VERSION 3.19...3.20 FATAL_ERROR)

project(Basalt VERSION 0.4 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CXX_STANDARD "17" CACHE STRING
    "Default value for CXX_STANDARD property of targets")
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL
    "Default value for CXX_EXTENSIONS property of targets")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON CACHE BOOL
    "Default value for INTERPROCEDURAL_OPTIMIZATION of targets")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF CACHE BOOL
    "Default value for INTERPROCEDURAL_OPTIMIZATION_DEBUG of targets")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF CACHE BOOL
    "Default value for INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL of targets")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(Compiler)
include(FeatureSummary)

add_subdirectory("external" EXCLUDE_FROM_ALL)
add_subdirectory("libapi")
add_subdirectory("libruntime")
add_subdirectory("platformlibs/libwin32")
add_subdirectory("sandbox")

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Sandbox.Win32)

feature_summary(WHAT ALL)

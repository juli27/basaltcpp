cmake_minimum_required(VERSION 3.19 FATAL_ERROR)
project(Basalt VERSION 0.4 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(Compiler)
include(FeatureSummary)

set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckIPOSupported)
check_ipo_supported()
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory("external" EXCLUDE_FROM_ALL)
add_subdirectory("libapi")
add_subdirectory("libruntime")
add_subdirectory("platformlibs/libwin32")
add_subdirectory("sandbox")

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT Sandbox.Win32)

feature_summary(WHAT ALL)

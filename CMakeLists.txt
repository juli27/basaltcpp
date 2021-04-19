cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(Basalt VERSION 0.4 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(FeatureSummary)

set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckIPOSupported)
check_ipo_supported()
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Dependencies
find_package(DirectXSDK MODULE REQUIRED "d3dx9")
## TODO: move to find module?
set_package_properties(DirectXSDK PROPERTIES
  DESCRIPTION "Microsoft DirectX SDK"
  URL "https://www.microsoft.com/en-us/download/details.aspx?id=6812"
  PURPOSE "Dependency of the Direct3D 9 gfx backend"
)

# Needs to be linked into the final exe in order to enable it
add_library(ControlFlowGuard INTERFACE)
target_compile_options(ControlFlowGuard INTERFACE "/guard:cf")
target_link_options(ControlFlowGuard INTERFACE "/GUARD:CF")

# Strict C++17 conformance
add_library(Cpp17_Strict INTERFACE)
target_compile_features(Cpp17_Strict INTERFACE cxx_std_17)
target_compile_options(Cpp17_Strict INTERFACE "/permissive-" "/Zc:inline")

add_library(WarningsAsErrors INTERFACE)
target_compile_options(WarningsAsErrors INTERFACE "/WX")

add_library(CommonPrivate INTERFACE)
target_compile_definitions(CommonPrivate INTERFACE "_UNICODE" "UNICODE")
target_compile_options(CommonPrivate INTERFACE
  "/FC" "/fp:fast" "/Gy" "/sdl" "/W4"
  "$<$<CONFIG:Debug>:/GF>"
  "$<$<CONFIG:RelWithDebInfo>:/Ob2>"
  "$<$<CONFIG:Release>:/Zi>"
)

add_subdirectory("external" EXCLUDE_FROM_ALL)
add_subdirectory("api")
add_subdirectory("sandbox")
add_subdirectory("app")

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT App)

feature_summary(WHAT ALL)

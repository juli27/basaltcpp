cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project(Basalt VERSION 0.4 LANGUAGES CXX)

include(CheckIPOSupported)
check_ipo_supported()

add_library(CommonOptions INTERFACE)

target_compile_definitions(CommonOptions INTERFACE "_UNICODE" "UNICODE")
target_compile_options(CommonOptions INTERFACE
  /diagnostics:caret /FC /fp:fast /guard:cf /Gy /sdl /W4 /WX /Zc:inline
  /Zc:rvalueCast
  $<$<CONFIG:Debug>:/GF>
  $<$<CONFIG:RelWithDebInfo>:/Ob2>
)

add_subdirectory("external")
add_subdirectory("api")

add_subdirectory("sandbox")

add_subdirectory("app")

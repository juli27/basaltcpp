cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

project(Basalt VERSION 0.3 LANGUAGES CXX)

include(CheckIPOSupported)
check_ipo_supported()

add_library(CommonOptions INTERFACE)

target_compile_definitions(CommonOptions INTERFACE "_UNICODE" "UNICODE")
target_compile_options(CommonOptions INTERFACE
  /diagnostics:column /FC /fp:fast /guard:cf /Gy /sdl /W4 /WX /Zc:inline
  /Zc:rvalueCast
  $<$<CONFIG:Debug>:/GF>
  $<$<CONFIG:RelWithDebInfo>:/Ob2>
)

add_subdirectory("external")
add_subdirectory("runtime")

add_subdirectory("sandbox")

add_subdirectory("app")

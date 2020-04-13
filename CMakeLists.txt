cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Basalt VERSION 0.3)

add_executable(App WIN32)

target_sources(App PRIVATE "sandbox/App.manifest")

add_subdirectory("external")
add_subdirectory("runtime")

add_subdirectory("sandbox")

target_include_directories(App SYSTEM PRIVATE "$ENV{DXSDK_DIR}Include")

target_compile_definitions(App
  PUBLIC _UNICODE UNICODE
  PRIVATE
    BASALT_BUILD
    BASALT_DEBUG_BUILD
    _SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING)

target_compile_options(App PRIVATE
  /diagnostics:column /FC /fp:fast /GF /guard:cf /Gy /sdl /W4 /WX /Zc:inline
  /Zc:rvalueCast)

target_link_libraries(App PRIVATE
  Runtime
  Sandbox
  "d3d9"
  "d3dx9$<$<CONFIG:Debug>:d>")

target_link_directories(App PRIVATE $ENV{DXSDK_DIR}Lib/x64)

target_link_options(App PRIVATE /guard:cf /WX
  "$<$<NOT:$<CONFIG:Debug>>:/OPT:REF;/OPT:ICF>")

set_target_properties(App PROPERTIES
  OUTPUT_NAME "Sandbox$<$<CONFIG:Debug>:_Debug>"
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  INTERPROCEDURAL_OPTIMIZATION ON
  INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
  INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF
  VS_JUST_MY_CODE_DEBUGGING $<CONFIG:Debug>)

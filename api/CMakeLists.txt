add_library(api STATIC)

set(CURRENT_TARGET "api")

add_subdirectory("api")

target_link_libraries(api PUBLIC ExternalHeaderOnly DearImGui Fmt spdlog)

target_include_directories(api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(api
  PUBLIC _UNICODE UNICODE
  PRIVATE
    BASALT_BUILD
    BASALT_DEBUG_BUILD
    BASALT_DEV_BUILD
)

target_compile_features(api PUBLIC cxx_std_17)

target_compile_options(api
  PUBLIC
    /diagnostics:column /FC /fp:fast /GF /guard:cf /Gy /sdl /W4 /WX /Zc:inline /Zc:rvalueCast)

set_target_properties(api PROPERTIES
  STATIC_LIBRARY_OPTIONS "/WX"
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  INTERPROCEDURAL_OPTIMIZATION ON
  INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
  INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF
  VS_JUST_MY_CODE_DEBUGGING $<CONFIG:Debug>)

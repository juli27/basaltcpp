add_library(Runtime STATIC)

add_subdirectory("runtime")

target_link_libraries(Runtime PUBLIC ExternalHeaderOnly DearImGui Fmt)

target_include_directories(Runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(Runtime
  PUBLIC _UNICODE UNICODE
  PRIVATE
    BASALT_BUILD
    BASALT_DEBUG_BUILD
    BASALT_DEV_BUILD
    _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
)

target_compile_features(Fmt PUBLIC cxx_std_17)

target_compile_options(Runtime
  PUBLIC
    /diagnostics:column /FC /fp:fast /GF /guard:cf /Gy /sdl /W4 /WX /Zc:inline /Zc:rvalueCast)

set_target_properties(Runtime PROPERTIES
  STATIC_LIBRARY_OPTIONS "/WX"
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  INTERPROCEDURAL_OPTIMIZATION ON
  INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
  INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF
  VS_JUST_MY_CODE_DEBUGGING $<CONFIG:Debug>)

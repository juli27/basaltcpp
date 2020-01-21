add_library(Fmt STATIC)

target_sources(Fmt
  PRIVATE
    "src/format.cc"
    "include/fmt/format-inl.h"
  PUBLIC
    "include/fmt/core.h"
    "include/fmt/format.h")

target_include_directories(Fmt PUBLIC "include")

target_compile_definitions(Fmt PRIVATE _UNICODE UNICODE)

target_compile_features(Fmt PUBLIC cxx_std_17)

target_compile_options(Fmt
  PRIVATE
    /diagnostics:column /FC /fp:fast /GF /guard:cf /Gy /permissive- /sdl /W4 /WX
    /Zc:inline)

set_target_properties(Fmt PROPERTIES
  STATIC_LIBRARY_OPTIONS "/WX"
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  INTERPROCEDURAL_OPTIMIZATION ON
  INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
  INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF)

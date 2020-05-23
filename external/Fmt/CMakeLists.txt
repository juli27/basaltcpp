add_library(Fmt STATIC)

add_subdirectory("fmt")

target_include_directories(Fmt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

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

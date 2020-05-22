add_library(spdlog STATIC)

add_subdirectory("spdlog")

target_link_libraries(spdlog PUBLIC Fmt)

target_include_directories(spdlog PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(spdlog
  PRIVATE _UNICODE UNICODE
  PUBLIC SPDLOG_COMPILED_LIB
)

target_compile_features(spdlog PUBLIC cxx_std_17)

target_compile_options(spdlog
  PRIVATE
    /diagnostics:column /FC /fp:fast /GF /guard:cf /Gy /permissive- /sdl /W4 /WX
    /Zc:inline)

set_target_properties(spdlog PROPERTIES
  STATIC_LIBRARY_OPTIONS "/WX"
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  INTERPROCEDURAL_OPTIMIZATION ON
  INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
  INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF)

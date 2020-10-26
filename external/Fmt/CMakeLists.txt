add_library(Fmt STATIC)
set(CURRENT_TARGET Fmt)

add_subdirectory("fmt")

target_link_libraries(DearImGui PRIVATE CommonOptions)

target_include_directories(Fmt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_options(Fmt PRIVATE /permissive-)

set_target_properties(Fmt PROPERTIES
  STATIC_LIBRARY_OPTIONS "/WX"
  INTERPROCEDURAL_OPTIMIZATION ON
  INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
  INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF)

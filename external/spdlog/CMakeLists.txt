add_library(spdlog STATIC)
set(CURRENT_TARGET spdlog)

add_subdirectory("spdlog")

target_link_libraries(spdlog PUBLIC Fmt PRIVATE CommonOptions)

target_include_directories(spdlog PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(spdlog PUBLIC SPDLOG_COMPILED_LIB)

target_compile_options(spdlog PRIVATE "/permissive-")

set_target_properties(spdlog PROPERTIES
  STATIC_LIBRARY_OPTIONS "/WX"
  INTERPROCEDURAL_OPTIMIZATION ON
  INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
  INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF)

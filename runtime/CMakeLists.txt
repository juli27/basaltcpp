add_library(Runtime STATIC)

add_subdirectory("runtime")

target_link_libraries(Runtime PUBLIC ExternalHeaderOnly DearImGui Fmt)

target_include_directories(Runtime PUBLIC ".")

target_compile_definitions(Runtime
  PUBLIC _UNICODE UNICODE
  PRIVATE BS_BUILD BS_DEBUG_BUILD)

target_compile_features(Fmt PUBLIC cxx_std_17)

target_compile_options(Runtime
  PUBLIC
    /diagnostics:column /FC /fp:fast /GF /guard:cf /Gy /JMC /sdl /W4 /WX /Zc:inline /Zc:rvalueCast)

set_target_properties(Runtime PROPERTIES
  STATIC_LIBRARY_OPTIONS "/WX"
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  INTERPROCEDURAL_OPTIMIZATION ON
  INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
  INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF)

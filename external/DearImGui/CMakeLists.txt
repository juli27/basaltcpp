add_library(DearImGui STATIC)

add_subdirectory("imgui")

target_include_directories(DearImGui INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(DearImGui PRIVATE _UNICODE UNICODE)

target_compile_features(DearImGui PUBLIC cxx_std_17)

target_compile_options(DearImGui
  PRIVATE
    /diagnostics:column /FC /fp:fast /GF /guard:cf /Gy /sdl /W4 /WX /Zc:inline
    /Zc:rvalueCast)

set_target_properties(DearImGui PROPERTIES
  STATIC_LIBRARY_OPTIONS "/WX"
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  INTERPROCEDURAL_OPTIMIZATION ON
  INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
  INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF)

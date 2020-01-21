add_library(DearImGui STATIC)

target_sources(DearImGui
  PRIVATE
    "src/imgui.cpp"
    "src/imgui_demo.cpp"
    "src/imgui_draw.cpp"
    "src/imgui_widgets.cpp"
    "src/imgui_impl_dx9.cpp"
  PUBLIC
    "include/imgui/imgui.h"
    "include/imgui/imconfig.h"
    "include/imgui/imgui_internal.h"
    "include/imgui/imstb_rectpack.h"
    "include/imgui/imstb_textedit.h"
    "include/imgui/imstb_truetype.h"
    "include/imgui/imgui_impl_dx9.h")

target_include_directories(DearImGui PRIVATE "include/imgui"
  INTERFACE "include")

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

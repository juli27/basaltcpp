add_library(Sandbox STATIC)

add_subdirectory("sandbox")

target_compile_definitions(Sandbox PRIVATE _UNICODE UNICODE BASALT_DEBUG_BUILD)

target_compile_options(Sandbox
  PRIVATE
    /diagnostics:column /FC /fp:fast /GF /guard:cf /Gy /sdl /W4 /WX /w34191 /w44062 /Zc:inline /Zc:rvalueCast)

target_include_directories(Sandbox PRIVATE
  "../sandbox"
  "../runtime"
  "../external/include"
  "../external/DearImGui/include"
  "../external/Fmt/include")

set_target_properties(Sandbox PROPERTIES
  STATIC_LIBRARY_OPTIONS "/WX"
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  INTERPROCEDURAL_OPTIMIZATION ON
  INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
  INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF
  VS_JUST_MY_CODE_DEBUGGING $<CONFIG:Debug>)
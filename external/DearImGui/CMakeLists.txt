add_library(DearImGui STATIC)
set(CURRENT_TARGET DearImGui)

add_subdirectory("imgui")

target_link_libraries(DearImGui PRIVATE CommonOptions)

target_include_directories(DearImGui INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(DearImGui PROPERTIES
  STATIC_LIBRARY_OPTIONS "/WX"
  INTERPROCEDURAL_OPTIMIZATION ON
  INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
  INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF
)

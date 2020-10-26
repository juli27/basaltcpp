add_library(Sandbox STATIC)
set(CURRENT_TARGET Sandbox)

add_subdirectory("sandbox")

target_link_libraries(Sandbox PRIVATE CommonOptions api)

target_include_directories(Sandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(Sandbox PRIVATE
  BASALT_DEBUG_BUILD
  BASALT_DEV_BUILD
)

target_compile_options(Sandbox PRIVATE "/w34191" "/w44062")

set_target_properties(Sandbox PROPERTIES
  STATIC_LIBRARY_OPTIONS "/WX"
  INTERPROCEDURAL_OPTIMIZATION ON
  INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
  INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF
  VS_JUST_MY_CODE_DEBUGGING $<CONFIG:Debug>
)

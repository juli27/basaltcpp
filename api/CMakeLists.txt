add_library(api STATIC)
set(CURRENT_TARGET api)

add_subdirectory("api")

target_link_libraries(api PUBLIC ExternalHeaderOnly DearImGui Fmt spdlog PRIVATE CommonOptions)

target_include_directories(api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(api
  PRIVATE
    BASALT_BUILD
    BASALT_DEBUG_BUILD
    BASALT_DEV_BUILD
)

set_target_properties(api PROPERTIES
  STATIC_LIBRARY_OPTIONS "/WX"
  INTERPROCEDURAL_OPTIMIZATION ON
  INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
  INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF
  VS_JUST_MY_CODE_DEBUGGING $<CONFIG:Debug>
)

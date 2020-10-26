add_executable(App WIN32)
set(CURRENT_TARGET App)

# Dependencies
find_package(DirectXSDK MODULE REQUIRED d3dx9)

target_link_libraries(App PRIVATE
  CommonOptions
  api
  Sandbox
  "d3d9"
  DirectXSDK::d3dx9
)

target_include_directories(App PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(App
  PRIVATE
    BASALT_BUILD
    BASALT_DEBUG_BUILD
    BASALT_DEV_BUILD
)

target_link_options(App PRIVATE "/guard:cf" "/WX"
  "$<$<NOT:$<CONFIG:Debug>>:/OPT:REF;/OPT:ICF>")

set_target_properties(App PROPERTIES
  OUTPUT_NAME "Sandbox$<$<CONFIG:Debug>:_Debug>"
  INTERPROCEDURAL_OPTIMIZATION ON
  INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
  INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF
  VS_JUST_MY_CODE_DEBUGGING $<CONFIG:Debug>)

target_sources(App PRIVATE "App.manifest")

add_subdirectory("app")

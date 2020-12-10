add_executable(App WIN32)
add_executable(Basalt::App ALIAS App)
set(CURRENT_TARGET App)

target_link_libraries(${CURRENT_TARGET} PRIVATE
  ControlFlowGuard
  CommonPrivate
  Basalt::API
  Basalt::Sandbox
  "d3d9"
  DirectXSDK::d3dx9
)

target_include_directories(${CURRENT_TARGET} PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_features(${CURRENT_TARGET} PRIVATE cxx_std_17)
target_compile_definitions(${CURRENT_TARGET} PRIVATE
  "BASALT_BUILD"
  "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:BASALT_DEV_BUILD>"
  "$<$<CONFIG:Debug>:BASALT_DEBUG_BUILD>"
)

target_compile_options(${CURRENT_TARGET} PRIVATE
  "/w44062;/w14165;/w34191;/w44242" "/Zc:inline" "/Zc:rvalueCast"
)

# TODO: /LTCG:incremental with ninja
target_link_options(${CURRENT_TARGET} PRIVATE
  "/WX"
  "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/DEBUG:FASTLINK>"
  "$<$<NOT:$<CONFIG:Debug>>:/OPT:REF;/OPT:ICF>"
  "$<$<CONFIG:Release>:/DEBUG:FULL>"
)

set_target_properties(${CURRENT_TARGET} PROPERTIES
  OUTPUT_NAME "Sandbox"
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  VS_JUST_MY_CODE_DEBUGGING "$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>"
)

add_subdirectory("app")

target_sources(${CURRENT_TARGET} PRIVATE "App.manifest")

get_target_property(TARGET_SOURCES ${CURRENT_TARGET} SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${TARGET_SOURCES})

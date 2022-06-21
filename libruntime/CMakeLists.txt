add_library(LibRuntime STATIC)
add_library(Basalt::LibRuntime ALIAS LibRuntime)
set(CURRENT_TARGET LibRuntime)

target_link_libraries(${CURRENT_TARGET}
  PUBLIC
    Basalt::LibAPI
  PRIVATE
    ControlFlowGuard
    CommonPrivate
)

target_include_directories(${CURRENT_TARGET} PUBLIC
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

set_target_properties(${CURRENT_TARGET} PROPERTIES
  STATIC_LIBRARY_OPTIONS "/WX"
  VS_JUST_MY_CODE_DEBUGGING "$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>"
)

add_subdirectory("basalt")

get_target_property(TARGET_SOURCES ${CURRENT_TARGET} SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/basalt" FILES ${TARGET_SOURCES})

add_library(LibRuntime STATIC)
add_library(Basalt::LibRuntime ALIAS LibRuntime)

target_link_libraries(LibRuntime
  PUBLIC
    Basalt::LibAPI
  PRIVATE
    Cpp17Conformance
    ControlFlowGuard
    CommonPrivate
)

target_include_directories(LibRuntime PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_features(LibRuntime PUBLIC cxx_std_17)
target_compile_definitions(LibRuntime PRIVATE
  "BASALT_BUILD"
  "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:BASALT_DEV_BUILD>"
  "$<$<CONFIG:Debug>:BASALT_DEBUG_BUILD>"
)

target_compile_options(LibRuntime PRIVATE
  "/w44062;/w14165;/w34191;/w44242"
)

set_property(TARGET LibRuntime PROPERTY STATIC_LIBRARY_OPTIONS "/WX")
set_property(TARGET LibRuntime PROPERTY VS_JUST_MY_CODE_DEBUGGING
              "$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>")

add_subdirectory("basalt")

get_property(TARGET_SOURCES TARGET LibRuntime PROPERTY SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/basalt" FILES ${TARGET_SOURCES})

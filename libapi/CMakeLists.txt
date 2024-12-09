add_library(LibAPI STATIC)
add_library(Basalt::LibAPI ALIAS LibAPI)

target_link_libraries(LibAPI
  PUBLIC
    DearImGui
    EnTT::EnTT
    Microsoft.GSL::GSL
    fmt::fmt
    spdlog::spdlog
  PRIVATE
    Cpp17Conformance
    CommonPrivate
    ControlFlowGuard
)

target_include_directories(LibAPI PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_definitions(LibAPI PRIVATE
  "BASALT_BUILD"
  "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:BASALT_DEV_BUILD>"
  "$<$<CONFIG:Debug>:BASALT_DEBUG_BUILD>"
)

target_compile_features(LibAPI PUBLIC cxx_std_17)
target_compile_options(LibAPI PRIVATE
  "/w44062" "/w14165" "/w34191" "/w44242"
)

set_property(TARGET LibAPI PROPERTY STATIC_LIBRARY_OPTIONS "/WX")
set_property(TARGET LibAPI PROPERTY VS_JUST_MY_CODE_DEBUGGING
              "$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>")

add_subdirectory("basalt/api")

get_property(TARGET_SOURCES TARGET LibAPI PROPERTY SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/basalt/api"
             FILES ${TARGET_SOURCES})

target_sources(LibAPI PRIVATE "libapi.natvis")

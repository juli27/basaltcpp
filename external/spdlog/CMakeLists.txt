add_library(spdlog STATIC)
set(CURRENT_TARGET spdlog)

target_link_libraries(${CURRENT_TARGET}
  PUBLIC
    fmt::fmt
  PRIVATE
    CommonPrivate
    Cpp17_Strict
    ControlFlowGuard
    WarningsAsErrors
)

target_include_directories(${CURRENT_TARGET} PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_definitions(${CURRENT_TARGET} PUBLIC "SPDLOG_COMPILED_LIB")

set_target_properties(${CURRENT_TARGET} PROPERTIES
  FOLDER "external"
  STATIC_LIBRARY_OPTIONS "/WX"
)

add_subdirectory("spdlog")

get_target_property(TARGET_SOURCES ${CURRENT_TARGET} SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${TARGET_SOURCES})

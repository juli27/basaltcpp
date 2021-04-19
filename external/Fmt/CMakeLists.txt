add_library(Fmt STATIC)
set(CURRENT_TARGET Fmt)

target_link_libraries(${CURRENT_TARGET}
  PRIVATE
    CommonPrivate
    Cpp17_Strict
    ControlFlowGuard
    WarningsAsErrors
)

target_include_directories(${CURRENT_TARGET}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

set_target_properties(${CURRENT_TARGET} PROPERTIES
  FOLDER "external"
  STATIC_LIBRARY_OPTIONS "/WX"
)

add_subdirectory("fmt")

get_target_property(TARGET_SOURCES ${CURRENT_TARGET} SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${TARGET_SOURCES})

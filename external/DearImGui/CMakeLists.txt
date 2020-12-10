add_library(DearImGui STATIC)
set(CURRENT_TARGET DearImGui)

target_link_libraries(${CURRENT_TARGET} PRIVATE
  CommonPrivate
  ControlFlowGuard
)

target_include_directories(${CURRENT_TARGET}
  INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_features(${CURRENT_TARGET} PRIVATE cxx_std_17)
target_compile_options(${CURRENT_TARGET} PRIVATE
  "/Zc:inline" "/Zc:rvalueCast"
)

set_target_properties(${CURRENT_TARGET} PROPERTIES
  FOLDER "external"
  STATIC_LIBRARY_OPTIONS "/WX"
)

add_subdirectory("imgui")

get_target_property(TARGET_SOURCES ${CURRENT_TARGET} SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${TARGET_SOURCES})

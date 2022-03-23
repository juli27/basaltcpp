FetchContent_GetProperties(DearImGui)
if (NOT dearimgui_POPULATED)
  FetchContent_Populate(DearImGui)

  file(COPY "imconfig.h" DESTINATION "${dearimgui_SOURCE_DIR}")

  add_library(DearImGui STATIC)
  set(CURRENT_TARGET DearImGui)

  target_link_libraries(${CURRENT_TARGET} PRIVATE
    CommonPrivate
    ControlFlowGuard
    WarningsAsErrors
  )

  target_include_directories(${CURRENT_TARGET}
    PRIVATE "${dearimgui_SOURCE_DIR}"
    INTERFACE "${dearimgui_BINARY_DIR}"
  )

  target_compile_features(${CURRENT_TARGET} PRIVATE cxx_std_17)
  target_compile_options(${CURRENT_TARGET} PRIVATE
    "/Zc:inline" "/Zc:rvalueCast"
  )

  set_target_properties(${CURRENT_TARGET} PROPERTIES
    FOLDER "external"
    STATIC_LIBRARY_OPTIONS "/WX"
  )

  target_sources(${CURRENT_TARGET} PRIVATE
    "${dearimgui_SOURCE_DIR}/imconfig.h"
    "${dearimgui_SOURCE_DIR}/imgui.cpp"
    "${dearimgui_SOURCE_DIR}/imgui.h"
    "${dearimgui_SOURCE_DIR}/imgui_demo.cpp"
    "${dearimgui_SOURCE_DIR}/imgui_draw.cpp"
    "${dearimgui_SOURCE_DIR}/imgui_internal.h"
    "${dearimgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${dearimgui_SOURCE_DIR}/imstb_rectpack.h"
    "${dearimgui_SOURCE_DIR}/imgui_tables.cpp"
    "${dearimgui_SOURCE_DIR}/imstb_textedit.h"
    "${dearimgui_SOURCE_DIR}/imstb_truetype.h"
    "${dearimgui_SOURCE_DIR}/backends/imgui_impl_dx9.cpp"
    "${dearimgui_SOURCE_DIR}/backends/imgui_impl_dx9.h"
  )

  file(
    COPY
      "${dearimgui_SOURCE_DIR}/imconfig.h"
      "${dearimgui_SOURCE_DIR}/imgui.h"
      "${dearimgui_SOURCE_DIR}/backends/imgui_impl_dx9.h"
    DESTINATION
      "${dearimgui_BINARY_DIR}/imgui"
  )

  get_target_property(TARGET_SOURCES ${CURRENT_TARGET} SOURCES)
  source_group(TREE "${dearimgui_SOURCE_DIR}" FILES ${TARGET_SOURCES})
endif()

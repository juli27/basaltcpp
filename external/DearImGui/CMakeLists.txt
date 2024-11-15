FetchContent_GetProperties(DearImGui)
if (NOT dearimgui_POPULATED)
  FetchContent_Populate(DearImGui)

  add_library(DearImGui STATIC)

  target_link_libraries(DearImGui
    PRIVATE
      Cpp17Conformance
      CommonPrivate
      ControlFlowGuard
  )

  target_include_directories(DearImGui
    PRIVATE "${dearimgui_SOURCE_DIR}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    INTERFACE "${dearimgui_BINARY_DIR}"
  )

  target_compile_definitions(DearImGui
    PUBLIC "IMGUI_USER_CONFIG=\"basalt_imconfig.h\""
  )

  target_compile_features(DearImGui PRIVATE cxx_std_17)
  target_compile_options(DearImGui PRIVATE "/Zc:twoPhase-" "/wd4505" "/wd4189")

  set_target_properties(DearImGui PROPERTIES
    FOLDER "external"
    STATIC_LIBRARY_OPTIONS "/WX"
  )

  target_sources(DearImGui
    PRIVATE
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

  get_target_property(TARGET_SOURCES DearImGui SOURCES)
  source_group(TREE "${dearimgui_SOURCE_DIR}" FILES ${TARGET_SOURCES})

  target_sources(DearImGui PRIVATE "basalt_imconfig.h")
  source_group("" FILES "basalt_imconfig.h")
endif()

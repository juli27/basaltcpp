FetchContent_MakeAvailable(DearImGui)

add_library(DearImGui STATIC)

target_link_libraries(DearImGui PRIVATE CommonFlags)

target_include_directories(DearImGui PUBLIC
  "${dearimgui_SOURCE_DIR}"
  "${dearimgui_SOURCE_DIR}/backends"
  "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(DearImGui PUBLIC
  "IMGUI_USER_CONFIG=\"basalt_imconfig.h\"")

target_compile_features(DearImGui PRIVATE cxx_std_17)
target_compile_options(DearImGui PRIVATE
  "/permissive-"
  "/Zc:twoPhase-"
  "/wd4505")

set_property(TARGET DearImGui PROPERTY FOLDER "external")

target_sources(DearImGui PRIVATE
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
  "${dearimgui_SOURCE_DIR}/backends/imgui_impl_dx9.h")

get_property(TARGET_SOURCES TARGET DearImGui PROPERTY SOURCES)
source_group(TREE "${dearimgui_SOURCE_DIR}" FILES ${TARGET_SOURCES})

target_sources(DearImGui PRIVATE "basalt_imconfig.h")
source_group("" FILES "basalt_imconfig.h")

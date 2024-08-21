add_subdirectory("scene")
add_subdirectory("gfx")

target_sources(${CURRENT_TARGET} PRIVATE
  "dear_imgui.cpp"
  "dear_imgui.h"
  "debug_ui.cpp"
  "engine.cpp"
  "input_manager.cpp"
  "input_manager.h"
  "input.cpp"
  "runtime.cpp"
  "runtime.h"
  "scene_view.cpp"
  "types.h"
  "view.cpp"
)

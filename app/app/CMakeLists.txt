add_subdirectory("base")
add_subdirectory("shared")
add_subdirectory("gfx")
add_subdirectory("d3d9")

target_sources(${CURRENT_TARGET} PRIVATE
  "app.cpp"
  "app.h"
  "build_config.h"
  "dear_imgui.cpp"
  "dear_imgui.h"
  "debug.cpp"
  "debug.h"
  "engine.cpp"
  "input_manager.cpp"
  "input_manager.h"
  "key_map.h"
  "main.cpp"
  "types.h"
  "util.cpp"
  "util.h"
  "window.cpp"
  "window.h"
)

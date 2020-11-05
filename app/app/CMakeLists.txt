add_subdirectory("shared")
add_subdirectory("gfx")
add_subdirectory("d3d9")

target_sources(${CURRENT_TARGET} PRIVATE
  "app.cpp"
  "debug.cpp"
  "engine.cpp"
  "main.cpp"
  "platform.cpp"
  "util.cpp"
  "window.cpp"
  
  "app.h"
  "build_config.h"
  "debug.h"
  "key_map.h"
  "types.h"
  "util.h"
  "window.h"
)

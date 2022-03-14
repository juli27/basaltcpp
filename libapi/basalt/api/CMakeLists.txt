target_sources(${CURRENT_TARGET} PRIVATE
  "client_app.h"
  "debug.cpp"
  "debug.h"
  "engine.h"
  "input_events.h"
  "prelude.h"
  "scene_view.cpp"
  "scene_view.h"
  "types.h"
  "view.cpp"
  "view.h"
)

add_subdirectory("base")
add_subdirectory("shared")
add_subdirectory("math")
add_subdirectory("scene")
add_subdirectory("gfx")

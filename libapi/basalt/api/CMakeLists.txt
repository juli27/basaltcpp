target_sources(LibAPI PRIVATE
  "bootstrap.h"
  "debug_ui.h"
  "engine.h"
  "input.h"
  "input_events.h"
  "prelude.h"
  "scene_view.h"
  "types.h"
  "view.h"
)

add_subdirectory("base")
add_subdirectory("shared")
add_subdirectory("math")
add_subdirectory("scene")
add_subdirectory("gfx")

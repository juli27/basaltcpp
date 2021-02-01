target_sources(${CURRENT_TARGET} PRIVATE
  "client_app.h"
  "debug.cpp"
  "debug.h"
  "engine.cpp"
  "engine.h"
  "input_events.h"
  "input_layer.cpp"
  "input_layer.h"
  "prelude.h"
  "types.h"
)

add_subdirectory("base")
add_subdirectory("shared")
add_subdirectory("math")
add_subdirectory("scene")
add_subdirectory("gfx")

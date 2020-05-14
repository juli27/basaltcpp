target_sources(Runtime
  PRIVATE
    "Camera.cpp"
    "Gfx.cpp"
  PUBLIC
    "Camera.h"
    "Gfx.h"
    "types.h")

add_subdirectory("backend")

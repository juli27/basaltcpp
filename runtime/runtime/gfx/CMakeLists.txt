target_sources(Runtime
  PRIVATE
    "Camera.cpp"
    "compositor.cpp"
    "draw_target.cpp"
    "scene_view.cpp"
    "visual.cpp"
  PUBLIC
    "Camera.h"
    "compositor.h"
    "draw_target.h"
    "drawable.h"
    "scene_view.h"
    "types.h"
    "visual.h"
)

add_subdirectory("backend")

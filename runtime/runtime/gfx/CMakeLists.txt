target_sources(Runtime
  PRIVATE
    "Camera.cpp"
    "Gfx.cpp"
  PUBLIC
    "Camera.h"
    "Gfx.h"
    "RenderComponent.h")

add_subdirectory("backend")

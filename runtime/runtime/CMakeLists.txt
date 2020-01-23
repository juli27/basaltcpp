target_sources(Runtime
  PRIVATE
    "Engine.cpp"
    "Input.cpp"
    "Scene.cpp"
  PUBLIC
    "Engine.h"
    "IApplication.h"
    "Input.h"
    "Prelude.h"
    "Scene.h")

add_subdirectory("gfx")
add_subdirectory("math")
add_subdirectory("platform")
add_subdirectory("shared")

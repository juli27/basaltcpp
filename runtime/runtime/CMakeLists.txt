target_sources(Runtime
  PRIVATE
    "dear_imgui.cpp"
    "Input.cpp"
    "Scene.cpp"
  PUBLIC
    "client_app.h"
    "dear_imgui.h"
    "Engine.h"
    "Input.h"
    "Prelude.h"
    "Scene.h")

add_subdirectory("gfx")
add_subdirectory("math")
add_subdirectory("platform")
add_subdirectory("scene")
add_subdirectory("shared")

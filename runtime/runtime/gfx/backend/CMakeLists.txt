target_sources(Runtime
  PRIVATE
    "render_command.cpp"
  PUBLIC
    "context.h"
    "IRenderer.h"
    "render_command.h"
    "Types.h")

add_subdirectory("d3d9")

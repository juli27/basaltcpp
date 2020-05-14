target_sources(Runtime
  PRIVATE
    "render_command.cpp"
    "Types.cpp"
  PUBLIC
    "context.h"
    "IRenderer.h"
    "RenderCommand.h"
    "Types.h")

add_subdirectory("d3d9")

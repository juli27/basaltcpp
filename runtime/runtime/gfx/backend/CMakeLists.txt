target_sources(Runtime
  PRIVATE
    "Types.cpp"
  PUBLIC
    "context.h"
    "IRenderer.h"
    "RenderCommand.h"
    "Types.h")

add_subdirectory("d3d9")

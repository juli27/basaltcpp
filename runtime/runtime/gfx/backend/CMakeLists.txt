target_sources(Runtime
  PRIVATE
    "Types.cpp"
  PUBLIC
    "IRenderer.h"
    "RenderCommand.h"
    "Types.h")

add_subdirectory("d3d9")

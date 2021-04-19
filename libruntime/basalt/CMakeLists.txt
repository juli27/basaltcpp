add_subdirectory("gfx")

add_subdirectory("api")

target_sources(${CURRENT_TARGET} PRIVATE
  "dear_imgui.cpp"
  "dear_imgui.h"
  "input_manager.cpp"
  "input_manager.h"
)

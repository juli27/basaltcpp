target_sources(${CURRENT_TARGET} PRIVATE
  "commands.h"
  "command_list.cpp"
  "command_list.h"
  "device.h"
  "render_state.cpp"
  "render_state.h"
  "types.h"
  "utils.h"
)

add_subdirectory("ext")

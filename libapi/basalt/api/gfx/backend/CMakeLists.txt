target_sources(${CURRENT_TARGET} PRIVATE
  "commands.h"
  "command_list.cpp"
  "command_list.h"
  "context.h"
  "device.h"
  "types.h"
  "utils.h"
)

add_subdirectory("ext")

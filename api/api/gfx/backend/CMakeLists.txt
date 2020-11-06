target_sources(${CURRENT_TARGET}
  PRIVATE
    "command_list.cpp"
  PUBLIC
    "commands.h"
    "command_list.h"
    "context.h"
    "device.h"
    "types.h"
    "utils.h"
)

add_subdirectory("ext")

target_sources(${CURRENT_TARGET}
  PRIVATE
    "command_list.cpp"
    "composite.cpp"
  PUBLIC
    "commands.h"
    "command_list.h"
    "composite.h"
    "context.h"
    "device.h"
    "types.h"
    "utils.h"
)

add_subdirectory("ext")

target_sources(${CURRENT_TARGET} PRIVATE
  "commands.cpp"
  "commands.h"
  "context.h"
  "device.h"
  "types.h"
  "validating_context.cpp"
  "validating_context.h"
  "validating_device.cpp"
  "validating_device.h"
)

add_subdirectory("ext")

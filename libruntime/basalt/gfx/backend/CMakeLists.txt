target_sources(${CURRENT_TARGET} PRIVATE
  "command_list.h"
  "commands.cpp"
  "commands.h"
  "device.h"
  "swap_chain.h"
  "types.h"
  "validating_device.cpp"
  "validating_device.h"
  "validating_swap_chain.cpp"
  "validating_swap_chain.h"
)

add_subdirectory("ext")

target_sources(LibRuntime PRIVATE
  "command_list.cpp"
  "command_list.h"
  "commands.cpp"
  "commands.h"
  "device.h"
  "factory.cpp"
  "factory.h"
  "swap_chain.h"
  "types.h"
  "validating_device.cpp"
  "validating_device.h"
  "validating_swap_chain.cpp"
  "validating_swap_chain.h"
  "vertex_layout.cpp"
)

add_subdirectory("ext")

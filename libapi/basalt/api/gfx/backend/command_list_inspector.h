#pragma once

#include "command_list.h"

#include <vector>

namespace basalt::gfx {

class CommandListInspector {
public:
  auto command_lists() const -> std::vector<CommandList> const&;
  auto set_command_lists(std::vector<CommandList>) -> void;
  
  auto show(bool& open) -> void;
  
private:
  std::vector<CommandList> mCommandLists;
};

} // namespace basalt::gfx

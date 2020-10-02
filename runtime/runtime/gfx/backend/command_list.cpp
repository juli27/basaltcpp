#include "command_list.h"

#include "commands.h"

using std::vector;

namespace basalt::gfx {

auto CommandList::commands() const noexcept -> const vector<CommandPtr>& {
  return mCommands;
}

} // namespace basalt::gfx

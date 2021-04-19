#include <basalt/api/gfx/backend/command_list.h>

using std::vector;

namespace basalt::gfx {

auto CommandList::commands() const noexcept -> const vector<CommandPtr>& {
  return mCommands;
}

} // namespace basalt::gfx

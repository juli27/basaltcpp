#include <basalt/api/gfx/backend/command_list.h>

using std::vector;

namespace basalt::gfx {

namespace {

// 128 KiB
constexpr uSize INITIAL_COMMAND_BUFFER_SIZE {128 * 1024};

} // namespace

CommandList::CommandList()
  : mBuffer {std::make_unique<CommandBuffer>(INITIAL_COMMAND_BUFFER_SIZE)} {
}

auto CommandList::commands() const noexcept -> const vector<Command*>& {
  return mCommands;
}

} // namespace basalt::gfx

#include "command_list.h"

#include "render_commands.h"

#include "runtime/shared/asserts.h"

#include <algorithm>
#include <array>

using std::array;
using std::vector;

namespace basalt::gfx {

CommandList::CommandList(
  const Mat4f32& view, const Mat4f32& projection, const Color& clearColor)
  : mView(view), mProjection(projection), mClearColor {clearColor} {
}

CommandList::~CommandList() = default;

auto CommandList::commands() const -> const vector<CommandPtr>& {
  return mCommands;
}

auto CommandList::view() const -> const Mat4f32& {
  return mView;
}

auto CommandList::projection() const -> const Mat4f32& {
  return mProjection;
}

auto CommandList::clear_color() const -> const Color& {
  return mClearColor;
}

void CommandList::add(const CommandLegacy& command) {
  mCommands.push_back(std::make_unique<CommandLegacy>(command));
}

void CommandList::set_ambient_light(const Color& color) {
  mCommands.push_back(std::make_unique<CommandSetAmbientLight>(color));
}

void CommandList::set_directional_lights(
  const std::vector<DirectionalLight>& lights
) {
  BASALT_ASSERT(lights.size() <= 4);

  array<DirectionalLight, 4> directionalLights {};
  std::copy(lights.begin(), lights.end(), directionalLights.begin());

  mCommands.push_back(
    std::make_unique<CommandSetDirectionalLights>(directionalLights));
}

} // namespace basalt::gfx

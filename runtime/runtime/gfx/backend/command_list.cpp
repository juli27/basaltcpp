#include "command_list.h"

#include "commands.h"

#include "runtime/math/mat4.h"

#include "runtime/shared/asserts.h"
#include <runtime/shared/color.h>

#include <algorithm>
#include <array>

using std::array;
using std::vector;

namespace basalt::gfx {

CommandList::~CommandList() = default;

auto CommandList::commands() const -> const vector<CommandPtr>& {
  return mCommands;
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

void CommandList::set_transform(
  const TransformType type, const Mat4f32& transform) {
  mCommands.push_back(std::make_unique<CommandSetTransform>(type, transform));
}

} // namespace basalt::gfx

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

auto CommandList::commands() const -> const vector<CommandPtr>& {
  return mCommands;
}

void CommandList::add(const CommandLegacy& command) {
  add<CommandLegacy>(command);
}

void CommandList::set_ambient_light(const Color& color) {
  add<CommandSetAmbientLight>(color);
}

void CommandList::set_directional_lights(
  const std::vector<DirectionalLight>& lights
) {
  BASALT_ASSERT(lights.size() <= 4);

  array<DirectionalLight, 4> directionalLights {};
  std::copy_n(
    lights.begin(), std::min(lights.size(), directionalLights.size())
  , directionalLights.begin());

  add<CommandSetDirectionalLights>(directionalLights);
}

void CommandList::set_transform(
  const TransformType type, const Mat4f32& transform) {
  add<CommandSetTransform>(type, transform);
}

void CommandList::set_render_state(const RenderState rs, const u32 value) {
  add<CommandSetRenderState>(rs, value);
}

} // namespace basalt::gfx

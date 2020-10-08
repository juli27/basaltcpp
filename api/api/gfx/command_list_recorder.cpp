#include "command_list_recorder.h"

#include "backend/commands.h"
#include "backend/ext/x_model_support.h"

#include <algorithm>
#include <array>
#include <utility>

using std::array;
using std::vector;

namespace basalt::gfx {

void CommandListRecorder::add(const CommandLegacy& command) {
  mCommandList.add<CommandLegacy>(command);
}

void CommandListRecorder::set_ambient_light(const Color& color) {
  mCommandList.add<CommandSetAmbientLight>(color);
}

void CommandListRecorder::set_directional_lights(
  const vector<DirectionalLight>& lights) {
  BASALT_ASSERT(lights.size() <= 4);

  array<DirectionalLight, 4> directionalLights {};
  std::copy_n(lights.begin(), std::min(lights.size(), directionalLights.size()),
              directionalLights.begin());

  mCommandList.add<CommandSetDirectionalLights>(directionalLights);
}

void CommandListRecorder::set_transform(const TransformType type,
                                        const Mat4f32& transform) {
  mCommandList.add<CommandSetTransform>(type, transform);
}

void CommandListRecorder::set_render_state(const RenderState state,
                                           const u32 value) {
  if (mDeviceState.update(state, value)) {
    mCommandList.add<CommandSetRenderState>(state, value);
  }
}

void CommandListRecorder::ext_draw_x_model(const ext::ModelHandle handle) {
  mCommandList.add<ext::CommandDrawXModel>(handle);
}

auto CommandListRecorder::complete_command_list() -> CommandList {
  return std::move(mCommandList);
}

} // namespace basalt::gfx

#include <basalt/api/gfx/command_list_recorder.h>

#include <basalt/api/gfx/backend/commands.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <algorithm>
#include <array>
#include <utility>

using std::array;
using std::vector;

namespace basalt::gfx {

void CommandListRecorder::clear(const Color& color) {
  mCommandList.add<CommandClear>(color);
}

void CommandListRecorder::draw(const VertexBuffer vertexBuffer,
                               const u32 startVertex,
                               const PrimitiveType primitiveType,
                               const u32 primitiveCount) {
  mCommandList.add<CommandDraw>(vertexBuffer, primitiveType, startVertex,
                                primitiveCount);
}

void CommandListRecorder::set_directional_lights(
  const vector<DirectionalLight>& lights) {
  BASALT_ASSERT(lights.size() <= 4);

  array<DirectionalLight, 4> directionalLights {};
  std::copy_n(lights.begin(), std::min(lights.size(), directionalLights.size()),
              directionalLights.begin());

  mCommandList.add<CommandSetDirectionalLights>(directionalLights);
}

void CommandListRecorder::set_transform(const TransformState state,
                                        const Mat4f32& transform) {
  if (mDeviceState.update(state, transform)) {
    mCommandList.add<CommandSetTransform>(state, transform);
  }
}

void CommandListRecorder::set_material(const Color& diffuse,
                                       const Color& ambient,
                                       const Color& emissive) {
  if (mDeviceState.update(diffuse, ambient, emissive)) {
    mCommandList.add<CommandSetMaterial>(diffuse, ambient, emissive);
  }
}

void CommandListRecorder::set_render_state(const RenderState& renderState) {
  if (mDeviceState.update(renderState)) {
    mCommandList.add<CommandSetRenderState>(renderState);
  }
}

void CommandListRecorder::set_texture(const Texture texture) {
  if (mDeviceState.update(texture)) {
    mCommandList.add<CommandSetTexture>(texture);
  }
}

void CommandListRecorder::set_texture_stage_state(const u8 stage,
                                                  const TextureStageState state,
                                                  const u32 value) {
  if (mDeviceState.update(state, value)) {
    mCommandList.add<CommandSetTextureStageState>(stage, state, value);
  }
}

void CommandListRecorder::ext_draw_x_model(const ext::XModel handle) {
  mCommandList.add<ext::CommandDrawXModel>(handle);
}

auto CommandListRecorder::take_cmd_list() -> CommandList {
  return std::move(mCommandList);
}

} // namespace basalt::gfx

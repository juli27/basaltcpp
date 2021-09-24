#include <basalt/api/gfx/filtering_command_list.h>

#include <basalt/api/gfx/backend/commands.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <algorithm>
#include <array>
#include <utility>

using std::array;
using std::vector;

namespace basalt::gfx {

void FilteringCommandList::clear(const Color& color) {
  mCommandList.add<CommandClear>(color);
}

void FilteringCommandList::draw(const VertexBuffer vertexBuffer,
                                const u32 startVertex,
                                const PrimitiveType primitiveType,
                                const u32 primitiveCount) {
  mCommandList.add<CommandDraw>(vertexBuffer, primitiveType, startVertex,
                                primitiveCount);
}

void FilteringCommandList::set_directional_lights(
  const vector<DirectionalLight>& lights) {
  BASALT_ASSERT(lights.size() <= 4);

  array<DirectionalLight, 4> directionalLights {};
  std::copy_n(lights.begin(), std::min(lights.size(), directionalLights.size()),
              directionalLights.begin());

  mCommandList.add<CommandSetDirectionalLights>(directionalLights);
}

void FilteringCommandList::set_transform(const TransformState state,
                                         const Mat4f32& transform) {
  if (mDeviceState.update(state, transform)) {
    mCommandList.add<CommandSetTransform>(state, transform);
  }
}

void FilteringCommandList::set_material(const Color& diffuse,
                                        const Color& ambient,
                                        const Color& emissive) {
  if (mDeviceState.update(diffuse, ambient, emissive)) {
    mCommandList.add<CommandSetMaterial>(diffuse, ambient, emissive);
  }
}

void FilteringCommandList::set_render_state(const RenderState& renderState) {
  if (mDeviceState.update(renderState)) {
    mCommandList.add<CommandSetRenderState>(renderState);
  }
}

void FilteringCommandList::bind_texture(const Texture texture) {
  if (mDeviceState.update(texture)) {
    mCommandList.add<CommandBindTexture>(texture);
  }
}

void FilteringCommandList::bind_sampler(const Sampler sampler) {
  if (mDeviceState.update(sampler)) {
    mCommandList.add<CommandBindSampler>(sampler);
  }
}

void FilteringCommandList::set_texture_stage_state(
  const u8 stage, const TextureStageState state, const u32 value) {
  if (mDeviceState.update(state, value)) {
    mCommandList.add<CommandSetTextureStageState>(stage, state, value);
  }
}

void FilteringCommandList::ext_draw_x_model(const ext::XModel handle) {
  mCommandList.add<ext::CommandDrawXModel>(handle);
}

auto FilteringCommandList::take_cmd_list() -> CommandList {
  return std::move(mCommandList);
}

} // namespace basalt::gfx

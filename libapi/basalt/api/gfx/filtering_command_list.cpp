#include <basalt/api/gfx/filtering_command_list.h>

#include <utility>

using std::vector;

namespace basalt::gfx {

void FilteringCommandList::clear(const Color& color) {
  mCommandList.clear(color);
}

void FilteringCommandList::draw(const VertexBuffer vertexBuffer,
                                const u32 startVertex,
                                const PrimitiveType primitiveType,
                                const u32 primitiveCount) {
  mCommandList.draw(vertexBuffer, startVertex, primitiveType, primitiveCount);
}

void FilteringCommandList::set_directional_lights(
  const vector<DirectionalLight>& lights) {
  mCommandList.set_directional_lights(lights);
}

void FilteringCommandList::set_transform(const TransformState state,
                                         const Mat4f32& transform) {
  if (mDeviceState.update(state, transform)) {
    mCommandList.set_transform(state, transform);
  }
}

void FilteringCommandList::set_material(const Color& diffuse,
                                        const Color& ambient,
                                        const Color& emissive) {
  if (mDeviceState.update(diffuse, ambient, emissive)) {
    mCommandList.set_material(diffuse, ambient, emissive);
  }
}

void FilteringCommandList::set_render_state(const RenderState& renderState) {
  if (mDeviceState.update(renderState)) {
    mCommandList.set_render_state(renderState);
  }
}

void FilteringCommandList::bind_texture(const Texture texture) {
  if (mDeviceState.update(texture)) {
    mCommandList.bind_texture(texture);
  }
}

void FilteringCommandList::bind_sampler(const Sampler sampler) {
  if (mDeviceState.update(sampler)) {
    mCommandList.bind_sampler(sampler);
  }
}

void FilteringCommandList::set_texture_stage_state(
  const u8 stage, const TextureStageState state, const u32 value) {
  if (mDeviceState.update(state, value)) {
    mCommandList.set_texture_stage_state(stage, state, value);
  }
}

void FilteringCommandList::ext_draw_x_model(const ext::XModel handle) {
  mCommandList.ext_draw_x_model(handle);
}

void FilteringCommandList::ext_render_dear_imgui() {
  mCommandList.ext_render_dear_imgui();
}

auto FilteringCommandList::take_cmd_list() -> CommandList {
  return std::move(mCommandList);
}

} // namespace basalt::gfx

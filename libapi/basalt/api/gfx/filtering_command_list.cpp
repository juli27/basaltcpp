#include <basalt/api/gfx/filtering_command_list.h>

#include <utility>

using gsl::span;

namespace basalt::gfx {

void FilteringCommandList::clear_attachments(const Attachments attachments,
                                             const Color& color, const f32 z,
                                             const u32 stencil) {
  mCommandList.clear_attachments(attachments, color, z, stencil);
}

void FilteringCommandList::draw(const u32 firstVertex, const u32 vertexCount) {
  mCommandList.draw(firstVertex, vertexCount);
}

void FilteringCommandList::set_render_state(const RenderState& renderState) {
  if (mDeviceState.update(renderState)) {
    mCommandList.set_render_state(renderState);
  }
}

void FilteringCommandList::bind_pipeline(const Pipeline handle) {
  if (mDeviceState.update(handle)) {
    mCommandList.bind_pipeline(handle);
  }
}

void FilteringCommandList::bind_vertex_buffer(const VertexBuffer buffer,
                                              const u64 offset) {
  if (mDeviceState.update(buffer, offset)) {
    mCommandList.bind_vertex_buffer(buffer, offset);
  }
}

void FilteringCommandList::bind_sampler(const Sampler sampler) {
  if (mDeviceState.update(sampler)) {
    mCommandList.bind_sampler(sampler);
  }
}

void FilteringCommandList::bind_texture(const Texture texture) {
  if (mDeviceState.update(texture)) {
    mCommandList.bind_texture(texture);
  }
}

void FilteringCommandList::set_transform(const TransformState state,
                                         const Mat4f32& transform) {
  if (mDeviceState.update(state, transform)) {
    mCommandList.set_transform(state, transform);
  }
}

void FilteringCommandList::set_ambient_light(const Color& c) {
  if (mDeviceState.update_ambient_light(c)) {
    mCommandList.set_ambient_light(c);
  }
}

void FilteringCommandList::set_directional_lights(
  const span<const DirectionalLight> lights) {
  mCommandList.set_directional_lights(lights);
}

void FilteringCommandList::set_material(const Color& diffuse,
                                        const Color& ambient,
                                        const Color& emissive) {
  if (mDeviceState.update(diffuse, ambient, emissive)) {
    mCommandList.set_material(diffuse, ambient, emissive);
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

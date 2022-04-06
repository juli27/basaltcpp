#include <basalt/gfx/filtering_command_list.h>

#include <utility>

using gsl::span;

namespace basalt::gfx {

auto FilteringCommandList::clear_attachments(Attachments const attachments,
                                             Color const& color,
                                             f32 const depth, u32 const stencil)
  -> void {
  mCommandList.clear_attachments(attachments, color, depth, stencil);
}

auto FilteringCommandList::draw(u32 const firstVertex, u32 const vertexCount)
  -> void {
  mCommandList.draw(firstVertex, vertexCount);
}

auto FilteringCommandList::draw_indexed(i32 const vertexOffset,
                                        u32 const minIndex,
                                        u32 const numVertices,
                                        u32 const firstIndex,
                                        u32 const indexCount) -> void {
  mCommandList.draw_indexed(vertexOffset, minIndex, numVertices, firstIndex,
                            indexCount);
}

auto FilteringCommandList::bind_pipeline(PipelineHandle const handle) -> void {
  if (mDeviceState.update(handle)) {
    mCommandList.bind_pipeline(handle);
  }
}

auto FilteringCommandList::bind_vertex_buffer(VertexBufferHandle const buffer,
                                              u64 const offsetInBytes) -> void {
  if (mDeviceState.update(buffer, offsetInBytes)) {
    mCommandList.bind_vertex_buffer(buffer, offsetInBytes);
  }
}

auto FilteringCommandList::bind_index_buffer(IndexBufferHandle const handle)
  -> void {
  if (mDeviceState.update(handle)) {
    mCommandList.bind_index_buffer(handle);
  }
}

auto FilteringCommandList::bind_sampler(u8 const slot,
                                        SamplerHandle const sampler) -> void {
  if (mDeviceState.update(slot, sampler)) {
    mCommandList.bind_sampler(slot, sampler);
  }
}

auto FilteringCommandList::bind_texture(u8 const slot,
                                        TextureHandle const texture) -> void {
  if (mDeviceState.update(slot, texture)) {
    mCommandList.bind_texture(slot, texture);
  }
}

auto FilteringCommandList::set_blend_constant(Color const& c) -> void {
  if (mDeviceState.update_blend_constant(c)) {
    mCommandList.set_blend_constant(c);
  }
}

auto FilteringCommandList::set_transform(TransformState const state,
                                         Matrix4x4f32 const& transform)
  -> void {
  if (mDeviceState.update(state, transform)) {
    mCommandList.set_transform(state, transform);
  }
}

auto FilteringCommandList::set_ambient_light(Color const& c) -> void {
  if (mDeviceState.update_ambient_light(c)) {
    mCommandList.set_ambient_light(c);
  }
}

auto FilteringCommandList::set_lights(span<LightData const> const lights)
  -> void {
  mCommandList.set_lights(lights);
}

auto FilteringCommandList::set_material(Color const& diffuse,
                                        Color const& ambient,
                                        Color const& emissive,
                                        Color const& specular,
                                        f32 const specularPower) -> void {
  if (mDeviceState.update(diffuse, ambient, emissive, specular,
                          specularPower)) {
    mCommandList.set_material(diffuse, ambient, emissive, specular,
                              specularPower);
  }
}

auto FilteringCommandList::set_fog_parameters(Color const& color,
                                              f32 const start, f32 const end,
                                              f32 const density) -> void {
  if (mDeviceState.update_fog_parameters(color, start, end, density)) {
    mCommandList.set_fog_parameters(color, start, end, density);
  }
}

auto FilteringCommandList::set_reference_alpha(u8 const alpha) -> void {
  if (mDeviceState.update_reference_alpha(alpha)) {
    mCommandList.set_reference_alpha(alpha);
  }
}

// TODO: Remove hack
auto FilteringCommandList::cmd_list() -> CommandList& {
  return mCommandList;
}

auto FilteringCommandList::take_cmd_list() -> CommandList {
  return std::move(mCommandList);
}

} // namespace basalt::gfx

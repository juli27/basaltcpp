#include <basalt/gfx/filtering_command_list.h>

#include <utility>

using gsl::span;

namespace basalt::gfx {

auto FilteringCommandList::clear_attachments(const Attachments attachments,
                                             const Color& color,
                                             const f32 depth, const u32 stencil)
  -> void {
  mCommandList.clear_attachments(attachments, color, depth, stencil);
}

auto FilteringCommandList::draw(const u32 firstVertex, const u32 vertexCount)
  -> void {
  mCommandList.draw(firstVertex, vertexCount);
}

auto FilteringCommandList::draw_indexed(const i32 vertexOffset,
                                        const u32 minIndex,
                                        const u32 numVertices,
                                        const u32 firstIndex,
                                        const u32 indexCount) -> void {
  mCommandList.draw_indexed(vertexOffset, minIndex, numVertices, firstIndex,
                            indexCount);
}

auto FilteringCommandList::bind_pipeline(const Pipeline handle) -> void {
  if (mDeviceState.update(handle)) {
    mCommandList.bind_pipeline(handle);
  }
}

auto FilteringCommandList::bind_vertex_buffer(const VertexBuffer buffer,
                                              const u64 offset) -> void {
  if (mDeviceState.update(buffer, offset)) {
    mCommandList.bind_vertex_buffer(buffer, offset);
  }
}
auto FilteringCommandList::bind_index_buffer(const IndexBuffer handle) -> void {
  if (mDeviceState.update(handle)) {
    mCommandList.bind_index_buffer(handle);
  }
}

auto FilteringCommandList::bind_sampler(const Sampler sampler) -> void {
  if (mDeviceState.update(sampler)) {
    mCommandList.bind_sampler(sampler);
  }
}

auto FilteringCommandList::bind_texture(const Texture texture) -> void {
  if (mDeviceState.update(texture)) {
    mCommandList.bind_texture(texture);
  }
}

auto FilteringCommandList::set_transform(const TransformState state,
                                         const Matrix4x4f32& transform)
  -> void {
  if (mDeviceState.update(state, transform)) {
    mCommandList.set_transform(state, transform);
  }
}

auto FilteringCommandList::set_ambient_light(const Color& c) -> void {
  if (mDeviceState.update_ambient_light(c)) {
    mCommandList.set_ambient_light(c);
  }
}

auto FilteringCommandList::set_lights(const span<const Light> lights) -> void {
  mCommandList.set_lights(lights);
}

auto FilteringCommandList::set_material(const Color& diffuse,
                                        const Color& ambient,
                                        const Color& emissive) -> void {
  if (mDeviceState.update(diffuse, ambient, emissive)) {
    mCommandList.set_material(diffuse, ambient, emissive);
  }
}

auto FilteringCommandList::ext_draw_x_mesh(const ext::XMesh handle,
                                           const u32 subsetIndex) -> void {
  mCommandList.ext_draw_x_mesh(handle, subsetIndex);
}

auto FilteringCommandList::ext_render_dear_imgui() -> void {
  mCommandList.ext_render_dear_imgui();
}

auto FilteringCommandList::take_cmd_list() -> CommandList {
  return std::move(mCommandList);
}

} // namespace basalt::gfx

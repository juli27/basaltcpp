#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/gfx/backend/commands.h>
#include <basalt/gfx/backend/command_list.h>

using gsl::span;

namespace basalt::gfx {

namespace {

// 128 KiB
constexpr uSize INITIAL_COMMAND_BUFFER_SIZE {128 * 1024};

} // namespace

CommandList::CommandList()
  : mBuffer {std::make_unique<CommandBuffer>(INITIAL_COMMAND_BUFFER_SIZE)} {
}

auto CommandList::size() const noexcept -> uSize {
  return mCommands.size();
}

auto CommandList::begin() const -> const_iterator {
  return mCommands.begin();
}

auto CommandList::end() const -> const_iterator {
  return mCommands.end();
}

auto CommandList::clear_attachments(const Attachments attachments,
                                    const Color& color, const f32 depth,
                                    const u32 stencil) -> void {
  add<CommandClearAttachments>(attachments, color, depth, stencil);
}

auto CommandList::draw(const u32 firstVertex, const u32 vertexCount) -> void {
  add<CommandDraw>(firstVertex, vertexCount);
}

auto CommandList::draw_indexed(const i32 vertexOffset, const u32 minIndex,
                               const u32 numVertices, const u32 firstIndex,
                               const u32 indexCount) -> void {
  add<CommandDrawIndexed>(vertexOffset, minIndex, numVertices, firstIndex,
                          indexCount);
}

auto CommandList::bind_pipeline(const Pipeline pipelineId) -> void {
  add<CommandBindPipeline>(pipelineId);
}

auto CommandList::bind_vertex_buffer(const VertexBuffer vertexBufferId,
                                     const u64 offsetInBytes) -> void {
  add<CommandBindVertexBuffer>(vertexBufferId, offsetInBytes);
}

auto CommandList::bind_index_buffer(const IndexBuffer indexBufferId) -> void {
  add<CommandBindIndexBuffer>(indexBufferId);
}

auto CommandList::bind_sampler(const Sampler samplerId) -> void {
  add<CommandBindSampler>(samplerId);
}

auto CommandList::bind_texture(const Texture textureId) -> void {
  add<CommandBindTexture>(textureId);
}

auto CommandList::set_transform(const TransformState transformState,
                                const Matrix4x4f32& transform) -> void {
  add<CommandSetTransform>(transformState, transform);
}

auto CommandList::set_ambient_light(const Color& ambientColor) -> void {
  add<CommandSetAmbientLight>(ambientColor);
}

auto CommandList::set_lights(const span<const LightData> lights) -> void {
  span lightsCopy {allocate<const LightData>(lights.size())};
  std::uninitialized_copy(lights.begin(), lights.end(), lightsCopy.begin());

  add<CommandSetLights>(lightsCopy);
}

auto CommandList::set_material(const Color& diffuse, const Color& ambient,
                               const Color& emissive, const Color& specular,
                               const f32 specularPower) -> void {
  add<CommandSetMaterial>(diffuse, ambient, emissive, specular, specularPower);
}

auto CommandList::set_fog_parameters(const Color& color, f32 start, f32 end,
                                     f32 density) -> void {
  add<CommandSetFogParameters>(color, start, end, density);
}

} // namespace basalt::gfx

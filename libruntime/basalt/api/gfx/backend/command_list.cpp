#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/gfx/backend/commands.h>
#include <basalt/gfx/backend/command_list.h>

using gsl::span;

namespace basalt::gfx {

namespace {

// 128 KiB
constexpr auto INITIAL_COMMAND_BUFFER_SIZE = uSize{128 * 1024};

} // namespace

CommandList::CommandList()
  : mBuffer{std::make_unique<CommandBuffer>(INITIAL_COMMAND_BUFFER_SIZE)} {
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

auto CommandList::clear_attachments(Attachments const attachments,
                                    Color const& color, f32 const depth,
                                    u32 const stencil) -> void {
  add<CommandClearAttachments>(attachments, color, depth, stencil);
}

auto CommandList::draw(u32 const firstVertex, u32 const vertexCount) -> void {
  add<CommandDraw>(firstVertex, vertexCount);
}

auto CommandList::draw_indexed(i32 const vertexOffset, u32 const minIndex,
                               u32 const numVertices, u32 const firstIndex,
                               u32 const indexCount) -> void {
  add<CommandDrawIndexed>(vertexOffset, minIndex, numVertices, firstIndex,
                          indexCount);
}

auto CommandList::bind_pipeline(PipelineHandle const pipelineId) -> void {
  add<CommandBindPipeline>(pipelineId);
}

auto CommandList::bind_vertex_buffer(VertexBufferHandle const vertexBufferId,
                                     u64 const offsetInBytes) -> void {
  add<CommandBindVertexBuffer>(vertexBufferId, offsetInBytes);
}

auto CommandList::bind_index_buffer(IndexBufferHandle const indexBufferId)
  -> void {
  add<CommandBindIndexBuffer>(indexBufferId);
}

auto CommandList::bind_sampler(u8 const slot, SamplerHandle const samplerId)
  -> void {
  add<CommandBindSampler>(slot, samplerId);
}

auto CommandList::bind_texture(u8 const slot, TextureHandle const textureId)
  -> void {
  add<CommandBindTexture>(slot, textureId);
}

auto CommandList::set_stencil_reference(u32 const value) -> void {
  add<CommandSetStencilReference>(value);
}

auto CommandList::set_stencil_read_mask(u32 const value) -> void {
  add<CommandSetStencilReadMask>(value);
}

auto CommandList::set_stencil_write_mask(u32 const value) -> void {
  add<CommandSetStencilWriteMask>(value);
}

auto CommandList::set_blend_constant(Color const& c) -> void {
  add<CommandSetBlendConstant>(c);
}

auto CommandList::set_transform(TransformState const transformState,
                                Matrix4x4f32 const& transform) -> void {
  add<CommandSetTransform>(transformState, transform);
}

auto CommandList::set_ambient_light(Color const& ambientColor) -> void {
  add<CommandSetAmbientLight>(ambientColor);
}

auto CommandList::set_lights(span<LightData const> const lights) -> void {
  auto const lightsCopy = allocate<LightData>(lights.size());
  std::uninitialized_copy(lights.begin(), lights.end(), lightsCopy.begin());

  add<CommandSetLights>(lightsCopy);
}

auto CommandList::set_material(Color const& diffuse, Color const& ambient,
                               Color const& emissive, Color const& specular,
                               f32 const specularPower) -> void {
  add<CommandSetMaterial>(diffuse, ambient, emissive, specular, specularPower);
}

auto CommandList::set_fog_parameters(Color const& color, f32 start, f32 end,
                                     f32 density) -> void {
  add<CommandSetFogParameters>(color, start, end, density);
}

auto CommandList::set_reference_alpha(u8 const alpha) -> void {
  add<CommandSetReferenceAlpha>(alpha);
}

auto CommandList::set_texture_factor(Color const& textureFactor) -> void {
  add<CommandSetTextureFactor>(textureFactor);
}

auto CommandList::set_texture_stage_constant(u8 const stageId,
                                             Color const& constant) -> void {
  add<CommandSetTextureStageConstant>(stageId, constant);
}

} // namespace basalt::gfx

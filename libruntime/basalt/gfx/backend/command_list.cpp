#include "command_list_p.h"

#include "commands.h"

namespace basalt::gfx {

namespace {

// 128 KiB
auto constexpr INITIAL_COMMAND_BUFFER_SIZE = 128 * uSize{1024};

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
  CommandListP::add<CommandClearAttachments>(*this, attachments, color, depth,
                                             stencil);
}

auto CommandList::draw(u32 const firstVertex, u32 const vertexCount) -> void {
  CommandListP::add<CommandDraw>(*this, firstVertex, vertexCount);
}

auto CommandList::draw_indexed(i32 const vertexOffset, u32 const minIndex,
                               u32 const numVertices, u32 const firstIndex,
                               u32 const indexCount) -> void {
  CommandListP::add<CommandDrawIndexed>(*this, vertexOffset, minIndex,
                                        numVertices, firstIndex, indexCount);
}

auto CommandList::bind_pipeline(PipelineHandle const pipelineId) -> void {
  CommandListP::add<CommandBindPipeline>(*this, pipelineId);
}

auto CommandList::bind_vertex_buffer(VertexBufferHandle const vertexBufferId,
                                     uDeviceSize const offsetInBytes) -> void {
  CommandListP::add<CommandBindVertexBuffer>(*this, vertexBufferId,
                                             offsetInBytes);
}

auto CommandList::bind_index_buffer(IndexBufferHandle const indexBufferId)
  -> void {
  CommandListP::add<CommandBindIndexBuffer>(*this, indexBufferId);
}

auto CommandList::bind_sampler(u8 const slot, SamplerHandle const samplerId)
  -> void {
  CommandListP::add<CommandBindSampler>(*this, slot, samplerId);
}

auto CommandList::bind_texture(u8 const slot, TextureHandle const textureId)
  -> void {
  CommandListP::add<CommandBindTexture>(*this, slot, textureId);
}

auto CommandList::set_stencil_reference(u32 const value) -> void {
  CommandListP::add<CommandSetStencilReference>(*this, value);
}

auto CommandList::set_stencil_read_mask(u32 const value) -> void {
  CommandListP::add<CommandSetStencilReadMask>(*this, value);
}

auto CommandList::set_stencil_write_mask(u32 const value) -> void {
  CommandListP::add<CommandSetStencilWriteMask>(*this, value);
}

auto CommandList::set_blend_constant(Color const& c) -> void {
  CommandListP::add<CommandSetBlendConstant>(*this, c);
}

auto CommandList::set_transform(TransformState const transformState,
                                Matrix4x4f32 const& transform) -> void {
  CommandListP::add<CommandSetTransform>(*this, transformState, transform);
}

auto CommandList::set_ambient_light(Color const& ambientColor) -> void {
  CommandListP::add<CommandSetAmbientLight>(*this, ambientColor);
}

auto CommandList::set_lights(gsl::span<LightData const> const lights) -> void {
  auto const lightsCopy =
    CommandListP::allocate<LightData>(*this, lights.size());
  std::uninitialized_copy(lights.begin(), lights.end(), lightsCopy.begin());

  CommandListP::add<CommandSetLights>(*this, lightsCopy);
}

auto CommandList::set_material(Color const& diffuse, Color const& ambient,
                               Color const& emissive, Color const& specular,
                               f32 const specularPower) -> void {
  CommandListP::add<CommandSetMaterial>(*this, diffuse, ambient, emissive,
                                        specular, specularPower);
}

auto CommandList::set_fog_parameters(Color const& color, f32 start, f32 end,
                                     f32 density) -> void {
  CommandListP::add<CommandSetFogParameters>(*this, color, start, end, density);
}

auto CommandList::set_reference_alpha(u8 const alpha) -> void {
  CommandListP::add<CommandSetReferenceAlpha>(*this, alpha);
}

auto CommandList::set_texture_factor(Color const& textureFactor) -> void {
  CommandListP::add<CommandSetTextureFactor>(*this, textureFactor);
}

auto CommandList::set_texture_stage_constant(u8 const stageId,
                                             Color const& constant) -> void {
  CommandListP::add<CommandSetTextureStageConstant>(*this, stageId, constant);
}

} // namespace basalt::gfx

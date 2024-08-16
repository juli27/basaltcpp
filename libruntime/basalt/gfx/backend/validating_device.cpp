#include "validating_device.h"

#include "commands.h"
#include "ext/dear_imgui_renderer.h"
#include "ext/texture_3d_support.h"

#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/vertex_layout.h>
#include <basalt/api/gfx/backend/ext/effect.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/log.h>

#include <gsl/span>

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string_view>
#include <utility>

using gsl::span;

using namespace std::literals;
using std::byte;
using std::shared_ptr;
using std::string_view;
using std::filesystem::path;

// TODO: expand validation
// - more extensive vertex layout validation
// - validate stride and offset in CommandBindVertexBuffer

namespace basalt::gfx {

namespace {

class ValidatingTexture3DSupport;
using ValidatingTexture3DSupportPtr = shared_ptr<ValidatingTexture3DSupport>;

class ValidatingTexture3DSupport final : public ext::Texture3DSupport {
public:
  static auto wrap(shared_ptr<Texture3DSupport> extension,
                   ValidatingDevice* device) -> ValidatingTexture3DSupportPtr {
    return std::make_shared<ValidatingTexture3DSupport>(std::move(extension),
                                                        device);
  }

  auto load(path const& path) -> TextureHandle override {
    auto const id = mExtension->load(path);

    return mDevice->construct_texture(id);
  }

  explicit ValidatingTexture3DSupport(shared_ptr<Texture3DSupport> extension,
                                      ValidatingDevice* device)
    : mExtension{std::move(extension)}
    , mDevice{device} {
  }

private:
  shared_ptr<Texture3DSupport> mExtension;
  ValidatingDevice* mDevice{};
};

auto check(string_view const description, bool const value) -> bool {
  if (!value) {
    BASALT_LOG_ERROR("check failed: {}", description);

    BASALT_CRASH("device validation failed");
  }

  return value;
}

template <typename Fn>
auto check(string_view const description, Fn&& fn) -> bool {
  return check(description, fn());
}

} // namespace

auto ValidatingDevice::wrap(DevicePtr device) -> ValidatingDevicePtr {
  return std::make_shared<ValidatingDevice>(std::move(device));
}

ValidatingDevice::ValidatingDevice(DevicePtr device)
  : mDevice{std::move(device)}
  , mCaps{mDevice->capabilities()} {
}

auto ValidatingDevice::wrap_extensions(ext::DeviceExtensions& deviceExtensions)
  -> void {
  if (deviceExtensions.count(ext::DeviceExtensionId::Texture3DSupport)) {
    auto ext = std::static_pointer_cast<ext::Texture3DSupport>(
      deviceExtensions.at(ext::DeviceExtensionId::Texture3DSupport));

    deviceExtensions[ext::DeviceExtensionId::Texture3DSupport] =
      ValidatingTexture3DSupport::wrap(std::move(ext), this);
  }
}

auto ValidatingDevice::construct_texture(TextureHandle const original)
  -> TextureHandle {
  return mTextures.allocate(TextureData{original});
}

auto ValidatingDevice::capabilities() const -> DeviceCaps const& {
  return mDevice->capabilities();
}

auto ValidatingDevice::get_status() const noexcept -> DeviceStatus {
  return mDevice->get_status();
}

auto ValidatingDevice::reset() -> void {
  mDevice->reset();
}

auto ValidatingDevice::create_pipeline(PipelineCreateInfo const& desc)
  -> PipelineHandle {
  if (desc.fragmentShader) {
    check("too many texture stages"sv,
          desc.fragmentShader->textureStages.size() <=
            mCaps.maxTextureBlendStages);
  }

  auto const pipelineId = mDevice->create_pipeline(desc);

  return mPipelines.allocate(PipelineData{
    pipelineId,
    VertexLayoutVector{desc.vertexLayout},
    desc.primitiveType,
  });
}

auto ValidatingDevice::destroy(PipelineHandle const id) noexcept -> void {
  if (!mPipelines.is_valid(id)) {
    return;
  }

  {
    auto const& data = mPipelines[id];

    mDevice->destroy(data.originalId);
  }

  mPipelines.deallocate(id);
}

auto ValidatingDevice::create_vertex_buffer(VertexBufferCreateInfo const& desc)
  -> VertexBufferHandle {
  check("non empty layout"sv, !desc.layout.empty());

  auto const& caps = mDevice->capabilities();
  check("not larger than max size"sv,
        desc.sizeInBytes <= caps.maxVertexBufferSizeInBytes);

  // From D3D9: FVF vertex buffers must be large enough to contain at least one
  // vertex, but it need not be a multiple of the vertex size
  auto const minSize = get_vertex_size_in_bytes(desc.layout);
  check("minimum size", desc.sizeInBytes >= minSize);

  auto const id = mDevice->create_vertex_buffer(desc);

  return mVertexBuffers.allocate(VertexBufferData{
    id,
    VertexLayoutVector{desc.layout},
    desc.sizeInBytes,
  });
}

auto ValidatingDevice::destroy(VertexBufferHandle const id) noexcept -> void {
  if (!mVertexBuffers.is_valid(id)) {
    return;
  }

  {
    auto const& data = mVertexBuffers[id];

    mDevice->destroy(data.originalId);
  }

  mVertexBuffers.deallocate(id);
}

auto ValidatingDevice::map(VertexBufferHandle const id,
                           uDeviceSize const offsetInBytes,
                           uDeviceSize const sizeInBytes) -> span<byte> {
  if (!check("valid vertex buffer id", mVertexBuffers.is_valid(id))) {
    return {};
  }

  auto const& data = mVertexBuffers[id];
  check("offset less than size", offsetInBytes < data.sizeInBytes);
  check("mapped region within bounds",
        offsetInBytes + sizeInBytes <= data.sizeInBytes);

  return mDevice->map(data.originalId, offsetInBytes, sizeInBytes);
}

auto ValidatingDevice::unmap(VertexBufferHandle const id) noexcept -> void {
  if (!check("valid vertex buffer id", mVertexBuffers.is_valid(id))) {
    return;
  }

  auto const& data = mVertexBuffers[id];

  mDevice->unmap(data.originalId);
}

auto ValidatingDevice::create_index_buffer(IndexBufferCreateInfo const& desc)
  -> IndexBufferHandle {
  auto const& caps = mDevice->capabilities();
  check("not larger than max size"sv,
        desc.sizeInBytes <= caps.maxIndexBufferSizeInBytes);
  check("index type supported", caps.supportedIndexTypes[desc.type]);

  auto const id = mDevice->create_index_buffer(desc);

  return mIndexBuffers.allocate(IndexBufferData{
    id,
    desc.sizeInBytes,
  });
}

auto ValidatingDevice::destroy(IndexBufferHandle const id) noexcept -> void {
  if (!mIndexBuffers.is_valid(id)) {
    return;
  }

  {
    auto const& data = mIndexBuffers[id];

    mDevice->destroy(data.originalId);
  }

  mIndexBuffers.deallocate(id);
}

auto ValidatingDevice::map(IndexBufferHandle const id,
                           uDeviceSize const offsetInBytes,
                           uDeviceSize const sizeInBytes) -> span<byte> {
  if (!check("valid vertex buffer id", mIndexBuffers.is_valid(id))) {
    return {};
  }

  auto const& data = mIndexBuffers[id];
  check("offset less than size", offsetInBytes < data.sizeInBytes);
  check("mapped region within bounds",
        offsetInBytes + sizeInBytes <= data.sizeInBytes);

  return mDevice->map(data.originalId, offsetInBytes, sizeInBytes);
}

auto ValidatingDevice::unmap(IndexBufferHandle const id) noexcept -> void {
  if (!check("valid vertex buffer id", mIndexBuffers.is_valid(id))) {
    return;
  }

  auto const& data = mIndexBuffers[id];

  mDevice->unmap(data.originalId);
}

auto ValidatingDevice::load_texture(path const& path) -> TextureHandle {
  auto const id = mDevice->load_texture(path);

  return mTextures.allocate(TextureData{
    id,
  });
}

auto ValidatingDevice::load_cube_texture(path const& path) -> TextureHandle {
  auto const id = mDevice->load_cube_texture(path);

  return mTextures.allocate(TextureData{id});
}

auto ValidatingDevice::destroy(TextureHandle const id) noexcept -> void {
  if (!mTextures.is_valid(id)) {
    return;
  }

  {
    auto const& data = mTextures[id];

    mDevice->destroy(data.originalId);
  }

  mTextures.deallocate(id);
}

auto ValidatingDevice::create_sampler(SamplerCreateInfo const& desc)
  -> SamplerHandle {
  auto const id = mDevice->create_sampler(desc);

  return mSamplers.allocate(SamplerData{
    id,
  });
}

auto ValidatingDevice::destroy(SamplerHandle const id) noexcept -> void {
  if (!mSamplers.is_valid(id)) {
    return;
  }

  {
    auto const& data = mSamplers[id];

    mDevice->destroy(data.originalId);
  }

  mSamplers.deallocate(id);
}

auto ValidatingDevice::submit(span<CommandList const> const commandLists)
  -> void {
  auto patchedComposite = Composite{};
  patchedComposite.reserve(commandLists.size());
  for (auto const& cmdList : commandLists) {
    patchedComposite.emplace_back(validate(cmdList));
  }

  mDevice->submit(patchedComposite);
}

auto ValidatingDevice::validate(CommandList const& cmdList) -> CommandList {
  auto patched = CommandList{};

  auto visitor = [&](auto&& cmd) {
    this->validate(std::forward<decltype(cmd)>(cmd));
    this->patch(patched, std::forward<decltype(cmd)>(cmd));
  };

  for (auto const* cmd : cmdList) {
    visit(*cmd, visitor);
  }

  return patched;
}

auto ValidatingDevice::validate(Command const&) -> void {
}

auto ValidatingDevice::validate(CommandClearAttachments const& cmd) -> void {
  if (cmd.attachments[Attachment::DepthBuffer]) {
    check("depth clear range", cmd.depth >= 0.0f && cmd.depth <= 1.0f);
  }
}

auto ValidatingDevice::validate(CommandDraw const&) -> void {
  check("no pipeline bound"sv, mPipelines.is_valid(mBoundPipeline));
}

auto ValidatingDevice::validate(CommandDrawIndexed const&) -> void {
  check("no indexed point list",
        mCurrentPrimitiveType != PrimitiveType::PointList);
}

auto ValidatingDevice::validate(CommandBindPipeline const& cmd) -> void {
  if (!check("valid pipeline id", mPipelines.is_valid(cmd.pipelineId))) {
    return;
  }

  mBoundPipeline = cmd.pipelineId;

  auto const& data = mPipelines[cmd.pipelineId];
  mCurrentPrimitiveType = data.primitiveType;
}

auto ValidatingDevice::validate(CommandBindVertexBuffer const& cmd) -> void {
  if (!check("valid vertex buffer id",
             mVertexBuffers.is_valid(cmd.vertexBufferId))) {
    return;
  }

  auto const& data = mVertexBuffers[cmd.vertexBufferId];
  auto const vertexSize = get_vertex_size_in_bytes(data.layout);
  auto const maxOffset = data.sizeInBytes - vertexSize;
  check("max offset", cmd.offsetInBytes < maxOffset);
}

auto ValidatingDevice::validate(CommandBindIndexBuffer const& cmd) -> void {
  check("valid index buffer id", mIndexBuffers.is_valid(cmd.indexBufferId));
}

auto ValidatingDevice::validate(CommandBindSampler const& cmd) -> void {
  check("valid sampler id", mSamplers.is_valid(cmd.samplerId));
}

auto ValidatingDevice::validate(CommandBindTexture const& cmd) -> void {
  check("valid texture id", mTextures.is_valid(cmd.textureId));
}

auto ValidatingDevice::validate(CommandSetTransform const& cmd) -> void {
  if (cmd.transformState == TransformState::ViewToClip) {
    check("(3,4) can't be negative in a projection matrix",
          cmd.transform.m34 >= 0);
  }
}

auto ValidatingDevice::validate(CommandSetAmbientLight const&) -> void {
}

auto ValidatingDevice::validate(CommandSetLights const& cmd) -> void {
  auto const& caps = mDevice->capabilities();
  check("max lights", cmd.lights.size() <= caps.maxLights);
}

auto ValidatingDevice::validate(CommandSetMaterial const&) -> void {
}

auto ValidatingDevice::validate(CommandSetFogParameters const&) -> void {
}

auto ValidatingDevice::patch(CommandList& cmdList, Command const& cmd) -> void {
  switch (cmd.type) {
  case CommandType::ExtDrawXMesh: {
    auto const& drawCmd = cmd.as<ext::CommandDrawXMesh>();
    ext::XMeshCommandEncoder::draw_x_mesh(cmdList, drawCmd.xMeshId);

    break;
  }

  case CommandType::ExtRenderDearImGui:
    ext::DearImGuiCommandEncoder::render_dear_imgui(cmdList);

    break;

  case CommandType::ExtBeginEffect: {
    auto const& beginCmd = cmd.as<ext::CommandBeginEffect>();
    ext::EffectCommandEncoder::begin_effect(cmdList, beginCmd.effect);

    break;
  }

  case CommandType::ExtEndEffect:
    ext::EffectCommandEncoder::end_effect(cmdList);

    break;

  case CommandType::ExtBeginEffectPass: {
    auto const& beginCmd = cmd.as<ext::CommandBeginEffectPass>();
    ext::EffectCommandEncoder::begin_effect_pass(cmdList, beginCmd.passIndex);

    break;
  }

  case CommandType::ExtEndEffectPass:
    ext::EffectCommandEncoder::end_effect_pass(cmdList);

    break;

  default:
    BASALT_CRASH("validating device can't handle this command");
  }
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandClearAttachments const& cmd) -> void {
  cmdList.clear_attachments(cmd.attachments, cmd.color, cmd.depth, cmd.stencil);
}

auto ValidatingDevice::patch(CommandList& cmdList, CommandDraw const& cmd)
  -> void {
  cmdList.draw(cmd.firstVertex, cmd.vertexCount);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandDrawIndexed const& cmd) -> void {
  cmdList.draw_indexed(cmd.vertexOffset, cmd.minIndex, cmd.numVertices,
                       cmd.firstIndex, cmd.indexCount);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandBindPipeline const& cmd) -> void {
  if (!mPipelines.is_valid(cmd.pipelineId)) {
    return;
  }

  auto const originalId = mPipelines[cmd.pipelineId].originalId;

  cmdList.bind_pipeline(originalId);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandBindVertexBuffer const& cmd) -> void {
  if (!mVertexBuffers.is_valid(cmd.vertexBufferId)) {
    return;
  }

  auto const originalId = mVertexBuffers[cmd.vertexBufferId].originalId;

  cmdList.bind_vertex_buffer(originalId, cmd.offsetInBytes);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandBindIndexBuffer const& cmd) -> void {
  if (!mIndexBuffers.is_valid(cmd.indexBufferId)) {
    return;
  }

  auto const originalId = mIndexBuffers[cmd.indexBufferId].originalId;

  cmdList.bind_index_buffer(originalId);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandBindSampler const& cmd) -> void {
  if (!mSamplers.is_valid(cmd.samplerId)) {
    return;
  }

  auto const originalId = mSamplers[cmd.samplerId].originalId;

  cmdList.bind_sampler(cmd.slot, originalId);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandBindTexture const& cmd) -> void {
  auto const originalId = mTextures[cmd.textureId].originalId;

  cmdList.bind_texture(cmd.slot, originalId);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandSetStencilReference const& cmd) -> void {
  cmdList.set_stencil_reference(cmd.value);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandSetStencilReadMask const& cmd) -> void {
  cmdList.set_stencil_read_mask(cmd.value);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandSetStencilWriteMask const& cmd) -> void {
  cmdList.set_stencil_write_mask(cmd.value);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandSetBlendConstant const& cmd) -> void {
  cmdList.set_blend_constant(cmd.value);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandSetTransform const& cmd) -> void {
  cmdList.set_transform(cmd.transformState, cmd.transform);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandSetAmbientLight const& cmd) -> void {
  cmdList.set_ambient_light(cmd.ambient);
}

auto ValidatingDevice::patch(CommandList& cmdList, CommandSetLights const& cmd)
  -> void {
  cmdList.set_lights(cmd.lights);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandSetMaterial const& cmd) -> void {
  cmdList.set_material(cmd.diffuse, cmd.ambient, cmd.emissive, cmd.specular,
                       cmd.specularPower);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandSetFogParameters const& cmd) -> void {
  cmdList.set_fog_parameters(cmd.color, cmd.start, cmd.end, cmd.density);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandSetReferenceAlpha const& cmd) -> void {
  cmdList.set_reference_alpha(cmd.value);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandSetTextureFactor const& cmd) -> void {
  cmdList.set_texture_factor(cmd.textureFactor);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             CommandSetTextureStageConstant const& cmd)
  -> void {
  cmdList.set_texture_stage_constant(cmd.stageId, cmd.constant);
}

} // namespace basalt::gfx

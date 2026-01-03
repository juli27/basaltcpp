#include "validating_device.h"

#include "commands.h"
#include "ext/dear_imgui_renderer.h"
#include "ext/texture_3d_support.h"

#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/pipeline.h>
#include <basalt/api/gfx/backend/vertex_layout.h>
#include <basalt/api/gfx/backend/ext/effect.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/base/asserts.h>
#include <basalt/api/base/log.h>

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
  return mTextures.emplace_at(original, TextureData{});
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

  auto const handle = mDevice->create_pipeline(desc);

  return mPipelines.emplace_at(handle, PipelineData{
                                         VertexLayoutVector{desc.vertexLayout},
                                         desc.primitiveType,
                                       });
}

auto ValidatingDevice::destroy(PipelineHandle const id) noexcept -> void {
  mPipelines.destroy(id);
  mDevice->destroy(id);
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

  return mVertexBuffers.emplace_at(id, VertexBufferData{
                                         VertexLayoutVector{desc.layout},
                                         desc.sizeInBytes,
                                       });
}

auto ValidatingDevice::destroy(VertexBufferHandle const id) noexcept -> void {
  mVertexBuffers.destroy(id);
  mDevice->destroy(id);
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

  return mDevice->map(id, offsetInBytes, sizeInBytes);
}

auto ValidatingDevice::unmap(VertexBufferHandle const id) noexcept -> void {
  if (!check("valid vertex buffer id", mVertexBuffers.is_valid(id))) {
    return;
  }

  mDevice->unmap(id);
}

auto ValidatingDevice::create_index_buffer(IndexBufferCreateInfo const& desc)
  -> IndexBufferHandle {
  auto const& caps = mDevice->capabilities();
  check("not larger than max size"sv,
        desc.sizeInBytes <= caps.maxIndexBufferSizeInBytes);
  check("index type supported", caps.supportedIndexTypes[desc.type]);

  auto const id = mDevice->create_index_buffer(desc);

  return mIndexBuffers.emplace_at(id, IndexBufferData{desc.sizeInBytes});
}

auto ValidatingDevice::destroy(IndexBufferHandle const id) noexcept -> void {
  mIndexBuffers.destroy(id);
  mDevice->destroy(id);
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

  return mDevice->map(id, offsetInBytes, sizeInBytes);
}

auto ValidatingDevice::unmap(IndexBufferHandle const id) noexcept -> void {
  if (!check("valid vertex buffer id", mIndexBuffers.is_valid(id))) {
    return;
  }

  mDevice->unmap(id);
}

auto ValidatingDevice::load_texture(path const& path) -> TextureHandle {
  auto const id = mDevice->load_texture(path);

  return mTextures.emplace_at(id);
}

auto ValidatingDevice::load_cube_texture(path const& path) -> TextureHandle {
  auto const id = mDevice->load_cube_texture(path);

  return mTextures.emplace_at(id);
}

auto ValidatingDevice::destroy(TextureHandle const id) noexcept -> void {
  mTextures.destroy(id);
  mDevice->destroy(id);
}

auto ValidatingDevice::create_sampler(SamplerCreateInfo const& desc)
  -> SamplerHandle {
  auto const id = mDevice->create_sampler(desc);

  return mSamplers.emplace_at(id);
}

auto ValidatingDevice::destroy(SamplerHandle const id) noexcept -> void {
  mSamplers.destroy(id);
  mDevice->destroy(id);
}

auto ValidatingDevice::submit(span<CommandList const> const commandLists)
  -> void {
  for (auto const& cmdList : commandLists) {
    validate(cmdList);
  }

  mDevice->submit(commandLists);
}

auto ValidatingDevice::validate(CommandList const& cmdList) -> void {
  auto visitor = [&](auto&& cmd) {
    this->validate(std::forward<decltype(cmd)>(cmd));
  };

  for (auto const* cmd : cmdList) {
    visit(*cmd, visitor);
  }
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

} // namespace basalt::gfx

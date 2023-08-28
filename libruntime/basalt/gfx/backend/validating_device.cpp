#include <basalt/gfx/backend/validating_device.h>

#include <basalt/gfx/backend/commands.h>
#include <basalt/gfx/backend/ext/dear_imgui_renderer.h>
#include <basalt/gfx/backend/ext/texture_3d_support.h>

#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/log.h>

#include <basalt/api/base/enum_array.h>
#include <basalt/api/base/types.h>

#include <gsl/span>

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <numeric>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

using gsl::span;

using namespace std::literals;
using std::byte;
using std::optional;
using std::string_view;
using std::vector;
using std::filesystem::path;

// TODO: expand validation
// - more extensive vertex layout validation
// - validate stride and offset in CommandBindVertexBuffer

namespace basalt::gfx {

namespace {

class ValidatingTexture3DSupport;
using ValidatingTexture3DSupportPtr =
  std::shared_ptr<ValidatingTexture3DSupport>;

class ValidatingTexture3DSupport final : public ext::Texture3DSupport {
public:
  static auto create(ValidatingDevice* device)
    -> ValidatingTexture3DSupportPtr {
    return std::make_shared<ValidatingTexture3DSupport>(device);
  }

  auto load(const path& path) -> Texture override {
    return mDevice->load_texture_3d(path);
  }

  explicit ValidatingTexture3DSupport(ValidatingDevice* device)
    : mDevice {device} {
  }

private:
  ValidatingDevice* mDevice {};
};

auto check(const string_view description, const bool value) -> bool {
  if (!value) {
    BASALT_LOG_ERROR("check failed: {}", description);

    BASALT_CRASH("device validation failed");
  }

  return value;
}

template <typename Fn>
auto check(const string_view description, Fn&& fn) -> bool {
  return check(description, fn());
}

auto check_vertex_layout(const VertexLayout layout) -> bool {
  return check(
    "can't use transformed positions with untransformed positions or "
    "normals"sv,
    [&] {
      const bool hasTransformedPosition {
        std::find(layout.begin(), layout.end(),
                  VertexElement::PositionTransformed4F32) != layout.end()};

      const bool hasUntransformedPosition {
        std::find(layout.begin(), layout.end(), VertexElement::Position3F32) !=
        layout.end()};
      const bool hasNormals {
        std::find(layout.begin(), layout.end(), VertexElement::Normal3F32) !=
        layout.end()};

      return !hasTransformedPosition ||
             (!hasUntransformedPosition && !hasNormals);
    });
}

auto get_size(const VertexElement vertexElement) -> uDeviceSize {
  static constexpr EnumArray<VertexElement, uDeviceSize, 10> SIZES {
    {VertexElement::Position3F32, 3 * sizeof(f32)},
    {VertexElement::PositionTransformed4F32, 4 * sizeof(f32)},
    {VertexElement::Normal3F32, 3 * sizeof(f32)},
    {VertexElement::PointSize1F32, sizeof(f32)},
    {VertexElement::ColorDiffuse1U32A8R8G8B8, sizeof(u32)},
    {VertexElement::ColorSpecular1U32A8R8G8B8, sizeof(u32)},
    {VertexElement::TextureCoords1F32, sizeof(f32)},
    {VertexElement::TextureCoords2F32, 2 * sizeof(f32)},
    {VertexElement::TextureCoords3F32, 3 * sizeof(f32)},
    {VertexElement::TextureCoords4F32, 4 * sizeof(f32)},
  };
  static_assert(SIZES.size() == VERTEX_ELEMENT_COUNT);

  return SIZES[vertexElement];
}

auto get_vertex_size(const VertexLayout vertexLayout) -> uDeviceSize {
  return std::accumulate(
    vertexLayout.begin(), vertexLayout.end(), uDeviceSize {0},
    [](const uDeviceSize size, const VertexElement element) {
      return size + get_size(element);
    });
}

} // namespace

auto ValidatingDevice::wrap(DevicePtr device) -> ValidatingDevicePtr {
  return std::make_shared<ValidatingDevice>(std::move(device));
}

ValidatingDevice::ValidatingDevice(DevicePtr device)
  : mDevice {std::move(device)}
  , mCaps {mDevice->capabilities()} {
  mExtensions[ext::ExtensionId::Texture3DSupport] =
    ValidatingTexture3DSupport::create(this);
}

auto ValidatingDevice::load_texture_3d(const path& path) -> Texture {
  const Texture id {
    mDevice->query_extension<ext::Texture3DSupport>().value()->load(path)};

  return mTextures.allocate(TextureData {id});
}

auto ValidatingDevice::capabilities() const -> const DeviceCaps& {
  return mDevice->capabilities();
}

auto ValidatingDevice::get_status() const noexcept -> DeviceStatus {
  return mDevice->get_status();
}

auto ValidatingDevice::reset() -> void {
  mDevice->reset();
}

auto ValidatingDevice::create_pipeline(const PipelineDescriptor& desc)
  -> Pipeline {
  check_vertex_layout(desc.vertexLayout);
  if (desc.fragmentShader) {
    check("too many texture stages"sv,
          desc.fragmentShader->textureStages.size() <=
            mCaps.maxTextureBlendStages);
  }

  const Pipeline pipelineId {mDevice->create_pipeline(desc)};

  vector<VertexElement> vertexInputLayout {desc.vertexLayout.begin(),
                                           desc.vertexLayout.end()};

  return mPipelines.allocate(PipelineData {
    pipelineId,
    std::move(vertexInputLayout),
    desc.primitiveType,
  });
}

auto ValidatingDevice::destroy(const Pipeline id) noexcept -> void {
  if (!mPipelines.is_valid(id)) {
    return;
  }

  {
    const auto& data {mPipelines[id]};

    mDevice->destroy(data.originalId);
  }

  mPipelines.deallocate(id);
}

auto ValidatingDevice::create_vertex_buffer(const VertexBufferDescriptor& desc,
                                            const span<const byte> initialData)
  -> VertexBuffer {
  check("non empty layout"sv, !desc.layout.empty());
  check_vertex_layout(desc.layout);

  const DeviceCaps& caps {mDevice->capabilities()};
  check("not larger than max size"sv,
        desc.sizeInBytes <= caps.maxVertexBufferSizeInBytes);

  // From D3D9: FVF vertex buffers must be large enough to contain at least one
  // vertex, but it need not be a multiple of the vertex size
  const uDeviceSize minSize {get_vertex_size(desc.layout)};
  check("minimum size", desc.sizeInBytes >= minSize);

  check("initial data fits"sv, initialData.size() <= desc.sizeInBytes);

  const VertexBuffer id {mDevice->create_vertex_buffer(desc, initialData)};
  vector<VertexElement> layout {desc.layout.begin(), desc.layout.end()};

  return mVertexBuffers.allocate(VertexBufferData {
    id,
    std::move(layout),
    desc.sizeInBytes,
  });
}

auto ValidatingDevice::destroy(const VertexBuffer id) noexcept -> void {
  if (!mVertexBuffers.is_valid(id)) {
    return;
  }

  {
    const auto& data {mVertexBuffers[id]};

    mDevice->destroy(data.originalId);
  }

  mVertexBuffers.deallocate(id);
}

auto ValidatingDevice::map(const VertexBuffer id, const uDeviceSize offset,
                           const uDeviceSize size) -> span<byte> {
  if (!check("valid vertex buffer id", mVertexBuffers.is_valid(id))) {
    return {};
  }

  const auto& data {mVertexBuffers[id]};
  check("offset less than size", offset < data.sizeInBytes);
  check("mapped region within bounds", offset + size <= data.sizeInBytes);

  return mDevice->map(data.originalId, offset, size);
}

auto ValidatingDevice::unmap(const VertexBuffer id) noexcept -> void {
  if (!check("valid vertex buffer id", mVertexBuffers.is_valid(id))) {
    return;
  }

  const auto& data {mVertexBuffers[id]};

  mDevice->unmap(data.originalId);
}

auto ValidatingDevice::create_index_buffer(const IndexBufferDescriptor& desc,
                                           const span<const byte> initialData)
  -> IndexBuffer {
  const DeviceCaps& caps {mDevice->capabilities()};
  check("not larger than max size"sv,
        desc.sizeInBytes <= caps.maxIndexBufferSizeInBytes);
  check("index type supported", caps.supportedIndexTypes[desc.type]);

  check("initial data fits"sv, initialData.size() <= desc.sizeInBytes);

  const IndexBuffer id {mDevice->create_index_buffer(desc, initialData)};

  return mIndexBuffers.allocate(IndexBufferData {
    id,
    desc.sizeInBytes,
  });
}

auto ValidatingDevice::destroy(const IndexBuffer id) noexcept -> void {
  if (!mIndexBuffers.is_valid(id)) {
    return;
  }

  {
    const auto& data {mIndexBuffers[id]};

    mDevice->destroy(data.originalId);
  }

  mIndexBuffers.deallocate(id);
}

auto ValidatingDevice::map(const IndexBuffer id,
                           const uDeviceSize offsetInBytes,
                           const uDeviceSize sizeInBytes) -> span<byte> {
  if (!check("valid vertex buffer id", mIndexBuffers.is_valid(id))) {
    return {};
  }

  const auto& data {mIndexBuffers[id]};
  check("offset less than size", offsetInBytes < data.sizeInBytes);
  check("mapped region within bounds",
        offsetInBytes + sizeInBytes <= data.sizeInBytes);

  return mDevice->map(data.originalId, offsetInBytes, sizeInBytes);
}

auto ValidatingDevice::unmap(const IndexBuffer id) noexcept -> void {
  if (!check("valid vertex buffer id", mIndexBuffers.is_valid(id))) {
    return;
  }

  const auto& data {mIndexBuffers[id]};

  mDevice->unmap(data.originalId);
}

auto ValidatingDevice::load_texture(const path& path) -> Texture {
  const Texture id {mDevice->load_texture(path)};

  return mTextures.allocate(TextureData {
    id,
  });
}

auto ValidatingDevice::load_cube_texture(const path& path) -> Texture {
  const Texture id {mDevice->load_cube_texture(path)};

  return mTextures.allocate(TextureData {id});
}

auto ValidatingDevice::destroy(const Texture id) noexcept -> void {
  if (!mTextures.is_valid(id)) {
    return;
  }

  {
    const auto& data {mTextures[id]};

    mDevice->destroy(data.originalId);
  }

  mTextures.deallocate(id);
}

auto ValidatingDevice::create_sampler(const SamplerDescriptor& desc)
  -> Sampler {
  const Sampler id {mDevice->create_sampler(desc)};

  return mSamplers.allocate(SamplerData {
    id,
  });
}

auto ValidatingDevice::destroy(const Sampler id) noexcept -> void {
  if (!mSamplers.is_valid(id)) {
    return;
  }

  {
    const auto& data {mSamplers[id]};

    mDevice->destroy(data.originalId);
  }

  mSamplers.deallocate(id);
}

auto ValidatingDevice::submit(const span<const CommandList> commandLists)
  -> void {
  Composite patchedComposite;
  for (const CommandList& cmdList : commandLists) {
    patchedComposite.emplace_back(validate(cmdList));
  }

  mDevice->submit(patchedComposite);
}

auto ValidatingDevice::query_extension(const ext::ExtensionId id)
  -> optional<ext::ExtensionPtr> {
  if (const auto entry = mExtensions.find(id); entry != mExtensions.end()) {
    return entry->second;
  }

  return mDevice->query_extension(id);
}

auto ValidatingDevice::validate(const CommandList& cmdList) -> CommandList {
  CommandList patched;

  auto visitor {[&](auto&& cmd) {
    this->validate(std::forward<decltype(cmd)>(cmd));
    this->patch(patched, std::forward<decltype(cmd)>(cmd));
  }};

  std::for_each(cmdList.begin(), cmdList.end(),
                [&](const Command* cmd) { visit(*cmd, visitor); });

  return patched;
}

auto ValidatingDevice::validate(const Command&) -> void {
}

auto ValidatingDevice::validate(const CommandClearAttachments& cmd) -> void {
  if (cmd.attachments[Attachment::DepthBuffer]) {
    check("depth clear range", cmd.depth >= 0.0f && cmd.depth <= 1.0f);
  }
}

auto ValidatingDevice::validate(const CommandDraw&) -> void {
  check("no pipeline bound"sv, mPipelines.is_valid(mBoundPipeline));
}

auto ValidatingDevice::validate(const CommandDrawIndexed&) -> void {
  check("no indexed point list",
        mCurrentPrimitiveType != PrimitiveType::PointList);
}

auto ValidatingDevice::validate(const CommandBindPipeline& cmd) -> void {
  if (!check("valid pipeline id", mPipelines.is_valid(cmd.pipelineId))) {
    return;
  }

  mBoundPipeline = cmd.pipelineId;

  const PipelineData& data {mPipelines[cmd.pipelineId]};
  mCurrentPrimitiveType = data.primitiveType;
}

auto ValidatingDevice::validate(const CommandBindVertexBuffer& cmd) -> void {
  if (!check("valid vertex buffer id",
             mVertexBuffers.is_valid(cmd.vertexBufferId))) {
    return;
  }

  const auto& data {mVertexBuffers[cmd.vertexBufferId]};
  const uDeviceSize vertexSize {get_vertex_size(data.layout)};
  const uDeviceSize maxOffset {data.sizeInBytes - vertexSize};
  check("max offset", cmd.offsetInBytes < maxOffset);
}

auto ValidatingDevice::validate(const CommandBindIndexBuffer& cmd) -> void {
  check("valid index buffer id", mIndexBuffers.is_valid(cmd.indexBufferId));
}

auto ValidatingDevice::validate(const CommandBindSampler& cmd) -> void {
  check("valid sampler id", mSamplers.is_valid(cmd.samplerId));
}

auto ValidatingDevice::validate(const CommandBindTexture& cmd) -> void {
  check("valid texture id", mTextures.is_valid(cmd.textureId));
}

auto ValidatingDevice::validate(const CommandSetTransform& cmd) -> void {
  if (cmd.transformState == TransformState::ViewToClip) {
    check("(3,4) can't be negative in a projection matrix",
          cmd.transform.m34 >= 0);
  }
}

auto ValidatingDevice::validate(const CommandSetAmbientLight&) -> void {
}

auto ValidatingDevice::validate(const CommandSetLights& cmd) -> void {
  const DeviceCaps& caps {mDevice->capabilities()};
  check("max lights", cmd.lights.size() <= caps.maxLights);
}

auto ValidatingDevice::validate(const CommandSetMaterial&) -> void {
}

auto ValidatingDevice::validate(const CommandSetFogParameters&) -> void {
}

auto ValidatingDevice::patch(CommandList& cmdList, const Command& cmd) -> void {
  switch (cmd.type) {
  case CommandType::ExtDrawXMesh: {
    const auto& drawCmd {cmd.as<ext::CommandDrawXMesh>()};
    ext::XMeshCommandEncoder::draw_x_mesh(cmdList, drawCmd.xMeshId);

    break;
  }

  case CommandType::ExtRenderDearImGui:
    ext::DearImGuiCommandEncoder::render_dear_imgui(cmdList);

    break;

  default:
    BASALT_CRASH("validating device can't handle this command");
  }
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandClearAttachments& cmd) -> void {
  cmdList.clear_attachments(cmd.attachments, cmd.color, cmd.depth, cmd.stencil);
}

auto ValidatingDevice::patch(CommandList& cmdList, const CommandDraw& cmd)
  -> void {
  cmdList.draw(cmd.firstVertex, cmd.vertexCount);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandDrawIndexed& cmd) -> void {
  cmdList.draw_indexed(cmd.vertexOffset, cmd.minIndex, cmd.numVertices,
                       cmd.firstIndex, cmd.indexCount);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandBindPipeline& cmd) -> void {
  if (!mPipelines.is_valid(cmd.pipelineId)) {
    return;
  }

  const Pipeline originalId {mPipelines[cmd.pipelineId].originalId};

  cmdList.bind_pipeline(originalId);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandBindVertexBuffer& cmd) -> void {
  if (!mVertexBuffers.is_valid(cmd.vertexBufferId)) {
    return;
  }

  const VertexBuffer originalId {mVertexBuffers[cmd.vertexBufferId].originalId};

  cmdList.bind_vertex_buffer(originalId, cmd.offsetInBytes);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandBindIndexBuffer& cmd) -> void {
  if (!mIndexBuffers.is_valid(cmd.indexBufferId)) {
    return;
  }

  const IndexBuffer originalId {mIndexBuffers[cmd.indexBufferId].originalId};

  cmdList.bind_index_buffer(originalId);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandBindSampler& cmd) -> void {
  if (!mSamplers.is_valid(cmd.samplerId)) {
    return;
  }

  const Sampler originalId {mSamplers[cmd.samplerId].originalId};

  cmdList.bind_sampler(cmd.slot, originalId);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandBindTexture& cmd) -> void {
  const Texture originalId {mTextures[cmd.textureId].originalId};

  cmdList.bind_texture(cmd.slot, originalId);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandSetStencilReference& cmd) -> void {
  cmdList.set_stencil_reference(cmd.value);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandSetStencilReadMask& cmd) -> void {
  cmdList.set_stencil_read_mask(cmd.value);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandSetStencilWriteMask& cmd) -> void {
  cmdList.set_stencil_write_mask(cmd.value);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandSetBlendConstant& cmd) -> void {
  cmdList.set_blend_constant(cmd.value);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandSetTransform& cmd) -> void {
  cmdList.set_transform(cmd.transformState, cmd.transform);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandSetAmbientLight& cmd) -> void {
  cmdList.set_ambient_light(cmd.ambient);
}

auto ValidatingDevice::patch(CommandList& cmdList, const CommandSetLights& cmd)
  -> void {
  cmdList.set_lights(cmd.lights);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandSetMaterial& cmd) -> void {
  cmdList.set_material(cmd.diffuse, cmd.ambient, cmd.emissive, cmd.specular,
                       cmd.specularPower);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandSetFogParameters& cmd) -> void {
  cmdList.set_fog_parameters(cmd.color, cmd.start, cmd.end, cmd.density);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandSetReferenceAlpha& cmd) -> void {
  cmdList.set_reference_alpha(cmd.value);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandSetTextureFactor& cmd) -> void {
  cmdList.set_texture_factor(cmd.textureFactor);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandSetTextureStageConstant& cmd)
  -> void {
  cmdList.set_texture_stage_constant(cmd.stageId, cmd.constant);
}

} // namespace basalt::gfx

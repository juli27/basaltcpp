#include <basalt/gfx/backend/validating_device.h>

#include <basalt/gfx/backend/commands.h>
#include <basalt/gfx/backend/utils.h>

#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/log.h>

#include <basalt/api/base/enum_array.h>
#include <basalt/api/base/types.h>

#include <gsl/span>

#include <algorithm>
#include <cstddef>
#include <filesystem>
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
  : mDevice {std::move(device)} {
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

auto ValidatingDevice::capabilities() const -> const DeviceCaps& {
  return mDevice->capabilities();
}

auto ValidatingDevice::create_pipeline(const PipelineDescriptor& desc)
  -> Pipeline {
  check_vertex_layout(desc.vertexInputState);
  check("max 1 texture stage"sv, desc.textureStages.size() <= 1u);

  const Pipeline pipelineId {mDevice->create_pipeline(desc)};

  vector<VertexElement> vertexInputLayout {desc.vertexInputState.begin(),
                                           desc.vertexInputState.end()};

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

auto ValidatingDevice::query_extension(const ext::ExtensionId id)
  -> optional<ext::ExtensionPtr> {
  return mDevice->query_extension(id);
}

auto ValidatingDevice::validate(const Command&) -> void {
}

auto ValidatingDevice::validate(const CommandClearAttachments& cmd) -> void {
  if (cmd.attachments[Attachment::DepthBuffer]) {
    check("depth clear range", cmd.depth >= 0.0f && cmd.depth <= 1.0f);
  }
}

auto ValidatingDevice::validate(const CommandDraw&) -> void {
}

auto ValidatingDevice::validate(const CommandDrawIndexed&) -> void {
  check("no indexed point list",
        mCurrentPrimitiveType != PrimitiveType::PointList);
}

auto ValidatingDevice::validate(const CommandBindPipeline& cmd) -> void {
  if (!check("valid pipeline id", mPipelines.is_valid(cmd.handle))) {
    return;
  }

  const PipelineData& data {mPipelines[cmd.handle]};
  mCurrentPrimitiveType = data.primitiveType;
}

auto ValidatingDevice::validate(const CommandBindVertexBuffer& cmd) -> void {
  if (!check("valid vertex buffer id", mVertexBuffers.is_valid(cmd.handle))) {
    return;
  }

  const auto& data {mVertexBuffers[cmd.handle]};
  const uDeviceSize vertexSize {get_vertex_size(data.layout)};
  const uDeviceSize maxOffset {data.sizeInBytes - vertexSize};
  check("max offset", cmd.offset < maxOffset);
}

auto ValidatingDevice::validate(const CommandBindIndexBuffer& cmd) -> void {
  check("valid index buffer id", mIndexBuffers.is_valid(cmd.handle));
}

auto ValidatingDevice::validate(const CommandBindSampler& cmd) -> void {
  check("valid sampler id", mSamplers.is_valid(cmd.sampler));
}

auto ValidatingDevice::validate(const CommandBindTexture& cmd) -> void {
  check("valid texture id", mTextures.is_valid(cmd.texture));
}

auto ValidatingDevice::validate(const CommandSetTransform& cmd) -> void {
  if (cmd.state == TransformState::ViewToViewport) {
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

auto ValidatingDevice::patch(CommandList& cmdList, const Command& cmd) -> void {
  switch (cmd.type) {
  case CommandType::ExtDrawXMesh: {
    const auto& drawCmd {cmd.as<ext::CommandDrawXMesh>()};
    cmdList.ext_draw_x_mesh(drawCmd.handle, drawCmd.subset);

    break;
  }

  case CommandType::ExtRenderDearImGui:
    cmdList.ext_render_dear_imgui();
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
  if (!mPipelines.is_valid(cmd.handle)) {
    return;
  }

  const Pipeline originalId {mPipelines[cmd.handle].originalId};

  cmdList.bind_pipeline(originalId);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandBindVertexBuffer& cmd) -> void {
  if (!mVertexBuffers.is_valid(cmd.handle)) {
    return;
  }

  const VertexBuffer originalId {mVertexBuffers[cmd.handle].originalId};

  cmdList.bind_vertex_buffer(originalId, cmd.offset);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandBindIndexBuffer& cmd) -> void {
  if (!mIndexBuffers.is_valid(cmd.handle)) {
    return;
  }

  const IndexBuffer originalId {mIndexBuffers[cmd.handle].originalId};

  cmdList.bind_index_buffer(originalId);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandBindSampler& cmd) -> void {
  if (!mSamplers.is_valid(cmd.sampler)) {
    return;
  }

  const Sampler originalId {mSamplers[cmd.sampler].originalId};

  cmdList.bind_sampler(originalId);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandBindTexture& cmd) -> void {
  if (!mTextures.is_valid(cmd.texture)) {
    return;
  }

  const Texture originalId {mTextures[cmd.texture].originalId};

  cmdList.bind_texture(originalId);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandSetTransform& cmd) -> void {
  cmdList.set_transform(cmd.state, cmd.transform);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandSetAmbientLight& cmd) -> void {
  cmdList.set_ambient_light(cmd.ambientColor);
}

auto ValidatingDevice::patch(CommandList& cmdList, const CommandSetLights& cmd)
  -> void {
  cmdList.set_lights(cmd.lights);
}

auto ValidatingDevice::patch(CommandList& cmdList,
                             const CommandSetMaterial& cmd) -> void {
  cmdList.set_material(cmd.diffuse, cmd.ambient, cmd.emissive);
}

} // namespace basalt::gfx
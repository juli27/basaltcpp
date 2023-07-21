#include <basalt/api/gfx/resource_cache.h>

#include <basalt/gfx/backend/device.h>
#include <basalt/gfx/backend/ext/x_model_support.h>

#include <basalt/api/base/types.h>

#include <algorithm>
#include <array>
#include <memory>
#include <utility>
#include <vector>

using std::array;
using std::byte;
using std::vector;
using std::filesystem::path;

using gsl::span;

namespace basalt::gfx {

auto ResourceCache::create(DevicePtr device) -> ResourceCachePtr {
  return std::make_shared<ResourceCache>(std::move(device));
}

ResourceCache::~ResourceCache() noexcept {
  // destroy the device resources for our compound resources
  for (const auto handle : mXModels) {
    destroy_data(handle);
  }

  for (const auto handle : mMeshes) {
    destroy_data(handle);
  }

  for (const auto handle : mMaterials) {
    destroy_data(handle);
  }

  for (const auto handle : mPipelines) {
    mDevice->destroy(handle);
  }

  for (const auto handle : mVertexBuffers) {
    mDevice->destroy(handle);
  }

  for (const auto handle : mIndexBuffers) {
    mDevice->destroy(handle);
  }

  for (const auto handle : mSamplers) {
    mDevice->destroy(handle);
  }

  for (const auto handle : mTextures) {
    mDevice->destroy(handle);
  }
}

auto ResourceCache::create_pipeline(const PipelineDescriptor& desc)
  -> Pipeline {
  const Pipeline handle {mDevice->create_pipeline(desc)};
  mPipelines.push_back(handle);

  return handle;
}

auto ResourceCache::destroy(const Pipeline handle) noexcept -> void {
  mPipelines.erase(std::remove(mPipelines.begin(), mPipelines.end(), handle),
                   mPipelines.end());

  mDevice->destroy(handle);
}

auto ResourceCache::create_vertex_buffer(const VertexBufferDescriptor& desc,
                                         const span<const byte> initialData)
  -> VertexBuffer {
  const VertexBuffer handle {mDevice->create_vertex_buffer(desc, initialData)};
  mVertexBuffers.push_back(handle);

  return handle;
}

auto ResourceCache::destroy(const VertexBuffer handle) noexcept -> void {
  mVertexBuffers.erase(
    std::remove(mVertexBuffers.begin(), mVertexBuffers.end(), handle),
    mVertexBuffers.end());

  mDevice->destroy(handle);
}

auto ResourceCache::create_index_buffer(const IndexBufferDescriptor& desc,
                                        const span<const std::byte> initialData)
  -> IndexBuffer {
  const IndexBuffer handle {mDevice->create_index_buffer(desc, initialData)};
  mIndexBuffers.push_back(handle);

  return handle;
}

auto ResourceCache::destroy(const IndexBuffer handle) noexcept -> void {
  mIndexBuffers.erase(
    std::remove(mIndexBuffers.begin(), mIndexBuffers.end(), handle),
    mIndexBuffers.end());

  mDevice->destroy(handle);
}

auto ResourceCache::create_sampler(const SamplerDescriptor& desc) -> Sampler {
  const Sampler handle {mDevice->create_sampler(desc)};
  mSamplers.push_back(handle);

  return handle;
}

auto ResourceCache::destroy(const Sampler handle) noexcept -> void {
  mSamplers.erase(std::remove(mSamplers.begin(), mSamplers.end(), handle),
                  mSamplers.end());

  mDevice->destroy(handle);
}

auto ResourceCache::load_texture(const path& path) -> Texture {
  const Texture handle {mDevice->load_texture(path)};
  mTextures.push_back(handle);

  return handle;
}

auto ResourceCache::destroy(const Texture handle) noexcept -> void {
  mTextures.erase(std::remove(mTextures.begin(), mTextures.end(), handle),
                  mTextures.end());

  mDevice->destroy(handle);
}

auto ResourceCache::load_x_model(const path& path) -> ext::XModel {
  // throws std::bad_optional_access if extension not present
  const auto modelExt {mDevice->query_extension<ext::XModelSupport>().value()};

  const ext::XModelData xModel {modelExt->load(path)};

  vector<Material> materials;
  materials.reserve(xModel.materials().size());

  for (const auto& material : xModel.materials()) {
    array<TextureBlendingStage, 1> textureStages {};
    PipelineDescriptor pipelineDesc;
    pipelineDesc.lightingEnabled = true;
    pipelineDesc.cullMode = CullMode::CounterClockwise;
    pipelineDesc.textureStages = textureStages;
    pipelineDesc.depthTest = TestPassCond::IfLessEqual;
    pipelineDesc.depthWriteEnable = true;
    MaterialDescriptor desc;
    desc.pipelineDesc = &pipelineDesc;
    desc.diffuse = material.diffuse;
    desc.ambient = material.ambient;

    if (!material.textureFile.empty()) {
      desc.sampledTexture.texture = load_texture(material.textureFile);
    }

    materials.push_back(create_material(desc));
  }

  return mXModels.allocate(XModelData {std::move(materials), xModel.mesh()});
}

auto ResourceCache::get(const ext::XModel handle) const -> const XModelData& {
  return mXModels[handle];
}

auto ResourceCache::destroy(const ext::XModel handle) noexcept -> void {
  destroy_data(handle);

  mXModels.deallocate(handle);
}

auto ResourceCache::create_mesh(const MeshDescriptor& desc) -> Mesh {
  const VertexBuffer vb {create_vertex_buffer(
    {desc.vertexData.size_bytes(), desc.layout}, desc.vertexData)};
  const IndexBuffer ib {
    !desc.indexData.empty()
      ? create_index_buffer({desc.indexData.size_bytes(), desc.indexType},
                            desc.indexData)
      : IndexBuffer::null()};

  return mMeshes.allocate(
    MeshData {vb, 0u, desc.vertexCount, ib, desc.indexCount});
}

auto ResourceCache::get(const Mesh handle) const -> const MeshData& {
  return mMeshes[handle];
}

auto ResourceCache::destroy(const Mesh handle) noexcept -> void {
  destroy_data(handle);

  mMeshes.deallocate(handle);
}

auto ResourceCache::create_material(const MaterialDescriptor& desc)
  -> Material {
  // TODO: cache pipelines
  const Pipeline pipeline {create_pipeline(*desc.pipelineDesc)};

  const u8 maxAnisotropy {
    desc.sampledTexture.filter == TextureFilter::Anisotropic
      ? mDevice->capabilities().samplerMaxAnisotropy
      : u8 {1},
  };

  // TODO: cache samplers
  const Sampler sampler {
    create_sampler(SamplerDescriptor {
      desc.sampledTexture.filter,
      desc.sampledTexture.filter,
      desc.sampledTexture.mipFilter,
      desc.sampledTexture.addressModeU,
      desc.sampledTexture.addressModeV,
      TextureAddressMode::Repeat,
      BorderColor::BlackTransparent,
      Color {},
      maxAnisotropy,
    }),
  };

  return mMaterials.allocate(MaterialData {
    desc.diffuse,
    desc.ambient,
    desc.emissive,
    desc.specular,
    desc.specularPower,
    desc.fogColor,
    desc.fogStart,
    desc.fogEnd,
    desc.fogDensity,
    pipeline,
    desc.sampledTexture.texture,
    sampler,
  });
}

auto ResourceCache::get(const Material material) const -> const MaterialData& {
  return mMaterials[material];
}

auto ResourceCache::destroy(const Material handle) noexcept -> void {
  destroy_data(handle);

  mMaterials.deallocate(handle);
}

ResourceCache::ResourceCache(DevicePtr device) : mDevice {std::move(device)} {
}

auto ResourceCache::map(const VertexBuffer vb, const uDeviceSize offset,
                        const uDeviceSize size) const -> span<std::byte> {
  return mDevice->map(vb, offset, size);
}

auto ResourceCache::unmap(const VertexBuffer vb) const -> void {
  mDevice->unmap(vb);
}

auto ResourceCache::map(const IndexBuffer ib, const uDeviceSize offsetInBytes,
                        const uDeviceSize sizeInBytes) const
  -> span<std::byte> {
  return mDevice->map(ib, offsetInBytes, sizeInBytes);
}

auto ResourceCache::unmap(const IndexBuffer ib) const -> void {
  mDevice->unmap(ib);
}

auto ResourceCache::destroy_data(const ext::XModel handle) noexcept -> void {
  if (!mXModels.is_valid(handle)) {
    return;
  }

  const auto& data {get(handle)};

  for (const Material material : data.materials) {
    destroy(material);
  }

  // throws std::bad_optional_access if extension not present
  const auto modelExt {*mDevice->query_extension<ext::XModelSupport>()};
  modelExt->destroy(data.mesh);
}

auto ResourceCache::destroy_data(const Mesh handle) noexcept -> void {
  if (!mMeshes.is_valid(handle)) {
    return;
  }

  auto& data {get(handle)};
  destroy(data.vertexBuffer);
  destroy(data.indexBuffer);
}

auto ResourceCache::destroy_data(const Material handle) noexcept -> void {
  const auto& data {get(handle)};

  destroy(data.sampler);
  destroy(data.pipeline);
}

} // namespace basalt::gfx

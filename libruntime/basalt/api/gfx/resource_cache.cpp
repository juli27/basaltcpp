#include <basalt/api/gfx/resource_cache.h>

#include <basalt/gfx/backend/device.h>
#include <basalt/gfx/backend/ext/effect.h>
#include <basalt/gfx/backend/ext/texture_3d_support.h>
#include <basalt/gfx/backend/ext/x_model_support.h>

#include <basalt/api/gfx/context.h>

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

auto ResourceCache::create(ContextPtr context) -> ResourceCachePtr {
  return std::make_shared<ResourceCache>(std::move(context));
}

ResourceCache::~ResourceCache() noexcept {
  if (auto const ext =
        mContext->query_device_extension<ext::Effects>().value_or(nullptr)) {
    for (auto const id : mEffects) {
      ext->destroy(id);
    }
  }

  // destroy the device resources for our compound resources
  for (auto const handle : mXModels) {
    destroy_data(handle);
  }

  for (auto const handle : mMeshes) {
    destroy_data(handle);
  }

  for (auto const handle : mMaterials) {
    destroy_data(handle);
  }

  for (auto const handle : mPipelines) {
    mDevice->destroy(handle);
  }

  for (auto const handle : mVertexBuffers) {
    mDevice->destroy(handle);
  }

  for (auto const handle : mIndexBuffers) {
    mDevice->destroy(handle);
  }

  for (auto const handle : mSamplers) {
    mDevice->destroy(handle);
  }

  for (auto const handle : mTextures) {
    mDevice->destroy(handle);
  }
}

auto ResourceCache::create_pipeline(PipelineDescriptor const& desc)
  -> Pipeline {
  auto const handle = mDevice->create_pipeline(desc);
  mPipelines.push_back(handle);

  return handle;
}

auto ResourceCache::destroy(Pipeline const handle) noexcept -> void {
  mPipelines.erase(std::remove(mPipelines.begin(), mPipelines.end(), handle),
                   mPipelines.end());

  mDevice->destroy(handle);
}

auto ResourceCache::create_vertex_buffer(VertexBufferDescriptor const& desc,
                                         span<byte const> const initialData)
  -> VertexBuffer {
  auto const handle = mDevice->create_vertex_buffer(desc, initialData);
  mVertexBuffers.push_back(handle);

  return handle;
}

auto ResourceCache::destroy(VertexBuffer const handle) noexcept -> void {
  mVertexBuffers.erase(
    std::remove(mVertexBuffers.begin(), mVertexBuffers.end(), handle),
    mVertexBuffers.end());

  mDevice->destroy(handle);
}

auto ResourceCache::create_index_buffer(IndexBufferDescriptor const& desc,
                                        span<std::byte const> const initialData)
  -> IndexBuffer {
  auto const handle = mDevice->create_index_buffer(desc, initialData);
  mIndexBuffers.push_back(handle);

  return handle;
}

auto ResourceCache::destroy(IndexBuffer const handle) noexcept -> void {
  mIndexBuffers.erase(
    std::remove(mIndexBuffers.begin(), mIndexBuffers.end(), handle),
    mIndexBuffers.end());

  mDevice->destroy(handle);
}

auto ResourceCache::create_sampler(SamplerDescriptor const& desc) -> Sampler {
  auto const handle = mDevice->create_sampler(desc);
  mSamplers.push_back(handle);

  return handle;
}

auto ResourceCache::destroy(Sampler const handle) noexcept -> void {
  mSamplers.erase(std::remove(mSamplers.begin(), mSamplers.end(), handle),
                  mSamplers.end());

  mDevice->destroy(handle);
}

auto ResourceCache::load_texture(path const& path) -> Texture {
  auto const handle = mDevice->load_texture(path);
  mTextures.push_back(handle);

  return handle;
}

auto ResourceCache::load_cube_texture(path const& path) -> Texture {
  auto const handle = mDevice->load_cube_texture(path);
  mTextures.push_back(handle);

  return handle;
}

auto ResourceCache::destroy(Texture const handle) noexcept -> void {
  mTextures.erase(std::remove(mTextures.begin(), mTextures.end(), handle),
                  mTextures.end());

  mDevice->destroy(handle);
}

auto ResourceCache::load_texture_3d(path const& path) -> Texture {
  // throws std::bad_optional_access if extension not present
  auto const tex3dExt =
    mContext->query_device_extension<ext::Texture3DSupport>().value();
  auto const handle = tex3dExt->load(path);
  mTextures.push_back(handle);

  return handle;
}

auto ResourceCache::load_x_model(path const& path) -> ext::XModel {
  // throws std::bad_optional_access if extension not present
  auto const modelExt =
    mContext->query_device_extension<ext::XModelSupport>().value();

  auto xModel = modelExt->load(path);

  auto materials = vector<Material>{};
  materials.reserve(xModel.materials.size());

  auto vs = FixedVertexShaderCreateInfo{};
  vs.lightingEnabled = true;

  auto fs = FixedFragmentShaderCreateInfo{};
  auto textureStages = array{TextureStage{}};
  fs.textureStages = textureStages;

  auto pipelineDesc = PipelineDescriptor{};
  pipelineDesc.vertexShader = &vs;
  pipelineDesc.fragmentShader = &fs;
  pipelineDesc.cullMode = CullMode::CounterClockwise;
  pipelineDesc.depthTest = TestPassCond::IfLessEqual;
  pipelineDesc.depthWriteEnable = true;

  for (auto const& material : xModel.materials) {
    auto desc = MaterialDescriptor{};
    desc.pipelineDesc = &pipelineDesc;
    desc.diffuse = material.diffuse;
    desc.ambient = material.ambient;
    desc.emissive = material.emissive;
    desc.specular = material.specular;
    desc.specularPower = material.specularPower;

    if (!material.textureFile.empty()) {
      desc.sampledTexture.texture = load_texture(material.textureFile);
    }

    materials.push_back(create_material(desc));
  }

  return mXModels.allocate(
    XModelData{std::move(xModel.meshes), std::move(materials)});
}

auto ResourceCache::load_x_model(XModelDescriptor const& desc) -> ext::XModel {
  // throws std::bad_optional_access if extension not present
  auto const modelExt =
    mContext->query_device_extension<ext::XModelSupport>().value();

  auto xModel = modelExt->load(desc.modelPath);

  auto const numModelMaterials =
    std::max(xModel.materials.size(), desc.materials.size());
  auto materials = vector<Material>{};
  materials.reserve(numModelMaterials);

  for (auto const& materialDesc : desc.materials) {
    materials.push_back(create_material(materialDesc));
  }

  // if the model has more materials than are provided in the XModelDescriptor
  // then use those
  if (desc.materials.size() < numModelMaterials) {
    auto vs = FixedVertexShaderCreateInfo{};
    vs.lightingEnabled = true;

    auto fs = FixedFragmentShaderCreateInfo{};
    auto textureStages = array{TextureStage{}};
    fs.textureStages = textureStages;

    auto pipelineDesc = PipelineDescriptor{};
    pipelineDesc.vertexShader = &vs;
    pipelineDesc.fragmentShader = &fs;
    pipelineDesc.cullMode = CullMode::CounterClockwise;
    pipelineDesc.depthTest = TestPassCond::IfLessEqual;
    pipelineDesc.depthWriteEnable = true;

    for (auto i = desc.materials.size(); i < numModelMaterials; ++i) {
      auto const& material = xModel.materials[i];
      auto materialDesc = MaterialDescriptor{};
      materialDesc.pipelineDesc = &pipelineDesc;
      materialDesc.diffuse = material.diffuse;
      materialDesc.ambient = material.ambient;

      if (!material.textureFile.empty()) {
        materialDesc.sampledTexture.texture =
          load_texture(material.textureFile);
      }

      materials.push_back(create_material(materialDesc));
    }
  }

  return mXModels.allocate(
    XModelData{std::move(xModel.meshes), std::move(materials)});
}

auto ResourceCache::get(ext::XModel const handle) const -> XModelData const& {
  return mXModels[handle];
}

auto ResourceCache::destroy(ext::XModel const handle) noexcept -> void {
  destroy_data(handle);

  mXModels.deallocate(handle);
}

auto ResourceCache::create_mesh(MeshDescriptor const& desc) -> Mesh {
  auto const vb = create_vertex_buffer(
    {desc.vertexData.size_bytes(), desc.layout}, desc.vertexData);
  auto const ib =
    !desc.indexData.empty()
      ? create_index_buffer({desc.indexData.size_bytes(), desc.indexType},
                            desc.indexData)
      : IndexBuffer::null();

  return mMeshes.allocate(
    MeshData{vb, 0u, desc.vertexCount, ib, desc.indexCount});
}

auto ResourceCache::get(Mesh const handle) const -> MeshData const& {
  return mMeshes[handle];
}

auto ResourceCache::destroy(Mesh const handle) noexcept -> void {
  destroy_data(handle);

  mMeshes.deallocate(handle);
}

auto ResourceCache::create_material(MaterialDescriptor const& desc)
  -> Material {
  // TODO: cache pipelines
  auto const pipeline = create_pipeline(*desc.pipelineDesc);

  auto const maxAnisotropy =
    desc.sampledTexture.filter == TextureFilter::Anisotropic
      ? mDevice->capabilities().samplerMaxAnisotropy
      : u8{1};

  // TODO: cache samplers
  auto const sampler = create_sampler({
    desc.sampledTexture.filter,
    desc.sampledTexture.filter,
    desc.sampledTexture.mipFilter,
    desc.sampledTexture.addressModeU,
    desc.sampledTexture.addressModeV,
    TextureAddressMode::Repeat,
    BorderColor::BlackTransparent,
    Color{},
    maxAnisotropy,
  });

  return mMaterials.allocate(MaterialData{
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

auto ResourceCache::get(Material const material) const -> MaterialData const& {
  return mMaterials[material];
}

auto ResourceCache::compile_effect(path const& filePath) -> ext::CompileResult {
  // throws std::bad_optional_access if extension not present
  auto const ext = mContext->query_device_extension<ext::Effects>().value();

  auto result = ext->compile(filePath);
  if (auto const* id = std::get_if<ext::EffectId>(&result)) {
    mEffects.push_back(*id);
  }

  return result;
}

auto ResourceCache::destroy(ext::EffectId const id) noexcept -> void {
  mEffects.erase(std::remove(mEffects.begin(), mEffects.end(), id),
                 mEffects.end());

  // throws std::bad_optional_access if extension not present
  auto const ext = mContext->query_device_extension<ext::Effects>().value();
  ext->destroy(id);
}

auto ResourceCache::get(ext::EffectId const id) const -> ext::Effect& {
  // throws std::bad_optional_access if extension not present
  auto const ext = mContext->query_device_extension<ext::Effects>().value();

  return ext->get(id);
}

auto ResourceCache::destroy(Material const handle) noexcept -> void {
  destroy_data(handle);

  mMaterials.deallocate(handle);
}

ResourceCache::ResourceCache(ContextPtr context)
  : mContext{std::move(context)}
  , mDevice{mContext->device()} {
}

auto ResourceCache::map(VertexBuffer const vb, uDeviceSize const offset,
                        uDeviceSize const size) const -> span<std::byte> {
  return mDevice->map(vb, offset, size);
}

auto ResourceCache::unmap(VertexBuffer const vb) const -> void {
  mDevice->unmap(vb);
}

auto ResourceCache::map(IndexBuffer const ib, uDeviceSize const offsetInBytes,
                        uDeviceSize const sizeInBytes) const
  -> span<std::byte> {
  return mDevice->map(ib, offsetInBytes, sizeInBytes);
}

auto ResourceCache::unmap(IndexBuffer const ib) const -> void {
  mDevice->unmap(ib);
}

auto ResourceCache::destroy_data(ext::XModel const handle) noexcept -> void {
  if (!mXModels.is_valid(handle)) {
    return;
  }

  auto const& data = get(handle);

  for (auto const material : data.materials) {
    destroy(material);
  }

  // throws std::bad_optional_access if extension not present
  auto const modelExt =
    mContext->query_device_extension<ext::XModelSupport>().value();
  for (auto const meshId : data.meshes) {
    modelExt->destroy(meshId);
  }
}

auto ResourceCache::destroy_data(Mesh const handle) noexcept -> void {
  if (!mMeshes.is_valid(handle)) {
    return;
  }

  auto& data = get(handle);
  destroy(data.vertexBuffer);
  destroy(data.indexBuffer);
}

auto ResourceCache::destroy_data(Material const handle) noexcept -> void {
  auto const& data = get(handle);

  destroy(data.sampler);
  destroy(data.pipeline);
}

} // namespace basalt::gfx

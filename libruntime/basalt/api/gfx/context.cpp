#include <basalt/api/gfx/context.h>

#include <basalt/gfx/backend/device.h>
#include <basalt/gfx/backend/ext/effect.h>
#include <basalt/gfx/backend/ext/texture_3d_support.h>
#include <basalt/gfx/backend/ext/x_model_support.h>

#if BASALT_DEV_BUILD
#include <basalt/gfx/backend/validating_device.h>
#include <basalt/gfx/backend/validating_swap_chain.h>
#endif

#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/shared/asserts.h>

#include <array>
#include <filesystem>
#include <memory>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

namespace basalt::gfx {

using gsl::span;

using std::array;
using std::nullopt;
using std::optional;
using std::vector;
using std::filesystem::path;

auto ContextCreateInfo::create_default(AdapterList const& adapters)
  -> ContextCreateInfo {
  auto const& adapter = adapters[0];

  return ContextCreateInfo{
    adapter.handle,       adapter.displayMode,   adapter.displayFormat,
    ImageFormat::Unknown, MultiSampleCount::One,
  };
}

auto Context::create(DevicePtr device, ext::DeviceExtensions deviceExtensions,
                     SwapChainPtr swapChain, Info info) -> ContextPtr {
  return std::make_shared<Context>(std::move(device),
                                   std::move(deviceExtensions),
                                   std::move(swapChain), std::move(info));
}

Context::Context(DevicePtr device, ext::DeviceExtensions deviceExtensions,
                 SwapChainPtr swapChain, Info info)
  : mDevice{std::move(device)}
  , mDeviceExtensions{std::move(deviceExtensions)}
  , mSwapChain{std::move(swapChain)}
  , mInfo{std::move(info)} {
  BASALT_ASSERT(mDevice);
  BASALT_ASSERT(mSwapChain);

#if BASALT_DEV_BUILD
  auto wrappedDevice = ValidatingDevice::wrap(std::move(mDevice));
  mDevice = wrappedDevice;
  wrappedDevice->wrap_extensions(mDeviceExtensions);
  mSwapChain =
    ValidatingSwapChain::wrap(std::move(mSwapChain), std::move(wrappedDevice));
#endif
}

auto Context::gfx_info() const noexcept -> Info const& {
  return mInfo;
}

auto Context::create_resource_cache() -> ResourceCachePtr {
  return ResourceCache::create(shared_from_this());
}

auto Context::create_pipeline(PipelineCreateInfo const& createInfo) const
  -> PipelineHandle {
  return mDevice->create_pipeline(createInfo);
}

auto Context::destroy(PipelineHandle const handle) const noexcept -> void {
  mDevice->destroy(handle);
}

auto Context::create_sampler(SamplerCreateInfo const& createInfo) const
  -> SamplerHandle {
  return mDevice->create_sampler(createInfo);
}

auto Context::destroy(SamplerHandle const handle) const noexcept -> void {
  mDevice->destroy(handle);
}

auto Context::load_texture_2d(path const& filePath) const -> TextureHandle {
  return mDevice->load_texture(filePath);
}

auto Context::load_texture_cube(path const& filePath) const -> TextureHandle {
  return mDevice->load_cube_texture(filePath);
}

auto Context::load_texture_3d(path const& filePath) const -> TextureHandle {
  // throws when absent
  auto const tex3dExt = query_device_extension<ext::Texture3DSupport>().value();

  return tex3dExt->load(filePath);
}

auto Context::destroy(TextureHandle const handle) const noexcept -> void {
  mDevice->destroy(handle);
}

auto Context::create_material(MaterialCreateInfo const& createInfo)
  -> MaterialHandle {
  return mMaterials.allocate(MaterialData{
    createInfo.diffuse,
    createInfo.ambient,
    createInfo.emissive,
    createInfo.specular,
    createInfo.specularPower,
    createInfo.fogColor,
    createInfo.fogStart,
    createInfo.fogEnd,
    createInfo.fogDensity,
    createInfo.pipeline,
    createInfo.sampledTexture.texture,
    createInfo.sampledTexture.sampler,
  });
}

auto Context::destroy(MaterialHandle const handle) noexcept -> void {
  mMaterials.deallocate(handle);
}

auto Context::get(MaterialHandle const handle) const -> MaterialData const& {
  return mMaterials[handle];
}

auto Context::compile_effect(path const& filePath) const -> ext::CompileResult {
  // throws std::bad_optional_access if extension not present
  auto const ext = query_device_extension<ext::Effects>().value();

  return ext->compile(filePath);
}

auto Context::destroy(ext::EffectId const handle) const noexcept -> void {
  auto const ext = query_device_extension<ext::Effects>().value();
  ext->destroy(handle);
}

auto Context::get(ext::EffectId const effectHandle) const -> ext::Effect& {
  // throws std::bad_optional_access if extension not present
  auto const ext = query_device_extension<ext::Effects>().value();

  return ext->get(effectHandle);
}

auto Context::create_vertex_buffer(
  VertexBufferCreateInfo const& createInfo,
  span<std::byte const> const initialData) const -> VertexBufferHandle {
  return mDevice->create_vertex_buffer(createInfo, initialData);
}

auto Context::destroy(VertexBufferHandle const handle) const noexcept -> void {
  mDevice->destroy(handle);
}

auto Context::create_index_buffer(IndexBufferCreateInfo const& createInfo,
                                  span<std::byte const> const initialData) const
  -> IndexBufferHandle {
  return mDevice->create_index_buffer(createInfo, initialData);
}

auto Context::destroy(IndexBufferHandle const handle) const noexcept -> void {
  mDevice->destroy(handle);
}

auto Context::create_mesh(MeshCreateInfo const& createInfo) -> MeshHandle {
  auto const vb = create_vertex_buffer(
    {createInfo.vertexData.size_bytes(), createInfo.layout},
    createInfo.vertexData);
  auto const ib = !createInfo.indexData.empty()
                    ? create_index_buffer({createInfo.indexData.size_bytes(),
                                           createInfo.indexType},
                                          createInfo.indexData)
                    : IndexBufferHandle::null();

  return mMeshes.allocate(
    MeshData{vb, 0u, createInfo.vertexCount, ib, createInfo.indexCount});
}

auto Context::destroy(MeshHandle const meshHandle) noexcept -> void {
  if (!mMeshes.is_valid(meshHandle)) {
    return;
  }

  {
    auto& data = get(meshHandle);
    destroy(data.vertexBuffer);
    destroy(data.indexBuffer);
  }

  mMeshes.deallocate(meshHandle);
}

auto Context::get(MeshHandle const meshHandle) const -> MeshData const& {
  return mMeshes[meshHandle];
}

auto Context::load_x_model(XModelLoadInfo const& loadInfo)
  -> ext::XModelHandle {
  // throws std::bad_optional_access if extension not present
  auto const modelExt = query_device_extension<ext::XModelSupport>().value();

  auto xModel = modelExt->load(loadInfo.filePath);

  auto const& overrideMaterials = loadInfo.materials;
  auto const numModelMaterials =
    std::max(xModel.materials.size(), overrideMaterials.size());
  auto materials = vector<MaterialHandle>{};
  materials.reserve(numModelMaterials);

  std::copy(overrideMaterials.begin(), overrideMaterials.end(),
            std::back_inserter(materials));

  // if the model has more materials than are provided in the XModelDescriptor
  // then use those
  if (overrideMaterials.size() < numModelMaterials) {
    // TODO: cache pipelines
    auto const pipeline = [&] {
      auto vs = FixedVertexShaderCreateInfo{};
      vs.lightingEnabled = true;

      auto fs = FixedFragmentShaderCreateInfo{};
      auto textureStages = array{TextureStage{}};
      fs.textureStages = textureStages;

      auto pipelineDesc = PipelineCreateInfo{};
      pipelineDesc.vertexShader = &vs;
      pipelineDesc.fragmentShader = &fs;
      pipelineDesc.cullMode = CullMode::CounterClockwise;
      pipelineDesc.depthTest = TestPassCond::IfLessEqual;
      pipelineDesc.depthWriteEnable = true;

      return create_pipeline(pipelineDesc);
    }();

    for (auto i = overrideMaterials.size(); i < numModelMaterials; ++i) {
      auto const& material = xModel.materials[i];
      auto materialDesc = MaterialCreateInfo{};
      materialDesc.pipeline = pipeline;
      materialDesc.diffuse = material.diffuse;
      materialDesc.ambient = material.ambient;

      if (!material.textureFile.empty()) {
        materialDesc.sampledTexture.texture =
          load_texture_2d(material.textureFile);
        // TODO: cache samplers
        materialDesc.sampledTexture.sampler = create_sampler({});
      }

      materials.push_back(create_material(materialDesc));
    }
  }

  return create_x_model({xModel.meshes, materials});
}

auto Context::create_x_model(ext::XModelCreateInfo const& createInfo)
  -> ext::XModelHandle {
  auto meshes = vector<ext::XMeshHandle>{createInfo.meshes.begin(),
                                         createInfo.meshes.end()};
  auto materials = vector<MaterialHandle>{createInfo.materials.begin(),
                                          createInfo.materials.end()};

  return mXModels.allocate(XModelData{std::move(meshes), std::move(materials)});
}

auto Context::destroy(ext::XModelHandle const xModelHandle) noexcept -> void {
  if (!mXModels.is_valid(xModelHandle)) {
    return;
  }

  {
    auto const& data = get(xModelHandle);

    for (auto const materialHandle : data.materials) {
      destroy(materialHandle);
    }

    // throws std::bad_optional_access if extension not present
    auto const modelExt = query_device_extension<ext::XModelSupport>().value();
    for (auto const xMeshHandle : data.meshes) {
      modelExt->destroy(xMeshHandle);
    }
  }

  mXModels.deallocate(xModelHandle);
}

auto Context::get(ext::XModelHandle const handle) const -> XModelData const& {
  return mXModels[handle];
}

auto Context::submit(span<CommandList const> const cmdLists) const -> void {
  mDevice->submit(cmdLists);
}

auto Context::device() const noexcept -> DevicePtr const& {
  return mDevice;
}

auto Context::swap_chain() const noexcept -> SwapChainPtr const& {
  return mSwapChain;
}

auto Context::query_device_extension(ext::DeviceExtensionId const id) const
  -> optional<ext::DeviceExtensionPtr> {
  if (auto const entry = mDeviceExtensions.find(id);
      entry != mDeviceExtensions.end()) {
    return entry->second;
  }

  return nullopt;
}

auto Context::map(VertexBufferHandle const vb, uDeviceSize const offsetInBytes,
                  uDeviceSize const sizeInBytes) const -> span<std::byte> {
  return mDevice->map(vb, offsetInBytes, sizeInBytes);
}

auto Context::unmap(VertexBufferHandle const vb) const -> void {
  mDevice->unmap(vb);
}

auto Context::map(IndexBufferHandle const ib, uDeviceSize const offsetInBytes,
                  uDeviceSize const sizeInBytes) const -> span<std::byte> {
  return mDevice->map(ib, offsetInBytes, sizeInBytes);
}

auto Context::unmap(IndexBufferHandle const ib) const -> void {
  mDevice->unmap(ib);
}

} // namespace basalt::gfx

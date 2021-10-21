#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/utils.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/resource_registry.h>

#include <utility>

using namespace std::literals;

namespace basalt::gfx {

namespace {

auto convert(const TextureCoordinateSource s) -> TexCoordinateSrc {
  switch (s) {
  case TextureCoordinateSource::Vertex:
    return TcsVertex;

  case TextureCoordinateSource::VertexPositionCameraSpace:
    return TcsVertexPositionCameraSpace;
  }

  return TcsVertex;
}

} // namespace

ResourceCache::ResourceCache(ResourceRegistryPtr resourceRegistry,
                             Device& device)
  : mResourceRegistry {std::move(resourceRegistry)}, mDevice {device} {
}

auto ResourceCache::is_loaded(const ResourceId id) const -> bool {
  return mModels.find(id) != mModels.end() ||
         mTextures.find(id) != mTextures.end();
}

auto ResourceCache::create_pipeline(const PipelineDescriptor& desc) const
  -> Pipeline {
  return mDevice.create_pipeline(desc);
}

void ResourceCache::destroy_pipeline(const Pipeline handle) const noexcept {
  mDevice.destroy_pipeline(handle);
}

auto ResourceCache::create_vertex_buffer(
  const VertexBufferDescriptor& desc,
  const gsl::span<const std::byte> initialData) const -> VertexBuffer {
  return mDevice.create_vertex_buffer(desc, initialData);
}

void ResourceCache::destroy_vertex_buffer(
  const VertexBuffer handle) const noexcept {
  mDevice.destroy_vertex_buffer(handle);
}

template <>
auto ResourceCache::load(const ResourceId id) -> ext::XModel {
  BASALT_ASSERT(!is_loaded(id), "XModel already loaded");

  const auto modelExt =
    *gfx::query_device_extension<ext::XModelSupport>(mDevice);

  const auto& path = mResourceRegistry->get_path(id);

  return mModels[id] = modelExt->load(path.u8string());
}

template <>
auto ResourceCache::load(const ResourceId id) -> Texture {
  BASALT_ASSERT(!is_loaded(id), "Texture already loaded");

  const auto& path = mResourceRegistry->get_path(id);
  return mTextures[id] = mDevice.load_texture(path);
}

auto ResourceCache::create_mesh(const MeshDescriptor& desc) -> Mesh {
  const VertexBufferDescriptor vbDesc {desc.data.size_bytes(), desc.layout};

  auto [mesh, data] = mMeshes.allocate();
  data.vertexBuffer = mDevice.create_vertex_buffer(vbDesc, desc.data);
  data.primitiveType = desc.primitiveType;
  data.primitiveCount = desc.primitiveCount;

  return mesh;
}

auto ResourceCache::create_material(const MaterialDescriptor& desc)
  -> Material {
  auto [handle, data] = mMaterials.allocate();

  data.pipeline = mDevice.create_pipeline(PipelineDescriptor {
    desc.lit,
    desc.cullBackFace ? CullMode::CounterClockwise : CullMode::None,
    DepthTestPass::IfLessEqual,
    true,
  });

  data.renderStates[RenderStateType::FillMode] =
    desc.solid ? FillMode::Solid : FillMode::Wireframe;

  data.textureStageStates[TextureStageState::CoordinateSource] =
    convert(desc.textureCoordinateSource);

  u32 value = 0;
  switch (desc.textureTransformMode) {
  case TextureTransformMode::Disabled:
    value = TtfDisabled;
    break;
  case TextureTransformMode::Count4:
    value = TtfCount4;
    break;
  }

  if (desc.textureTransformProjected) {
    value |= TtfProjected;
  }

  data.textureStageStates[TextureStageState::TextureTransformFlags] = value;

  data.diffuse = desc.diffuse;
  data.ambient = desc.ambient;
  data.texture = desc.sampledTexture.texture;

  // TODO: cache samplers
  const SamplerDescriptor samplerDesc {
    desc.sampledTexture.filter, desc.sampledTexture.mipFilter,
    desc.sampledTexture.addressModeU, desc.sampledTexture.addressModeV};
  data.sampler = mDevice.create_sampler(samplerDesc);

  return handle;
}

template <>
auto ResourceCache::get(const ResourceId id) -> ext::XModel {
  BASALT_ASSERT(mModels.find(id) != mModels.end());

  return mModels[id];
}

template <>
auto ResourceCache::get(const ResourceId id) -> Texture {
  BASALT_ASSERT(mTextures.find(id) != mTextures.end());

  return mTextures[id];
}

auto ResourceCache::get(const Mesh mesh) const -> const MeshData& {
  return mMeshes[mesh];
}

auto ResourceCache::get(const Material material) const -> const MaterialData& {
  return mMaterials[material];
}

} // namespace basalt::gfx

#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/device.h>
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
                             DevicePtr device)
  : mResourceRegistry {std::move(resourceRegistry)}
  , mDevice {std::move(device)} {
}

auto ResourceCache::is_loaded(const ResourceId id) const -> bool {
  return mModels.find(id) != mModels.end() ||
         mTextures.find(id) != mTextures.end();
}

template <>
auto ResourceCache::load(const ResourceId id) -> ext::XModel {
  BASALT_ASSERT(!is_loaded(id), "XModel already loaded");

  const auto modelExt =
    *gfx::query_device_extension<ext::XModelSupport>(*mDevice);

  const auto& path = mResourceRegistry->get_path(id);

  return mModels[id] = modelExt->load(path.u8string());
}

template <>
auto ResourceCache::load(const ResourceId id) -> Texture {
  BASALT_ASSERT(!is_loaded(id), "Texture already loaded");

  const auto& path = mResourceRegistry->get_path(id);
  return mTextures[id] = mDevice->add_texture(path.u8string());
}

auto ResourceCache::create_mesh(const MeshDescriptor& desc) -> Mesh {
  auto [mesh, data] = mMeshes.allocate();
  data.vertexBuffer = mDevice->create_vertex_buffer(desc.data, desc.layout);
  data.primitiveType = desc.primitiveType;
  data.primitiveCount = desc.primitiveCount;

  return mesh;
}

auto ResourceCache::create_material(const MaterialDescriptor& desc)
  -> Material {
  auto [handle, data] = mMaterials.allocate();

  data.renderStates[RenderStateType::CullMode] =
    desc.cullBackFace ? CullMode::CounterClockwise : CullMode::None;
  data.renderStates[RenderStateType::Lighting] = desc.lit;

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

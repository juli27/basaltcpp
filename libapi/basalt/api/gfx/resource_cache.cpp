#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/utils.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

using std::byte;
using std::filesystem::path;

using gsl::span;

namespace basalt::gfx {

ResourceCache::ResourceCache(Device& device) : mDevice {device} {
}

auto ResourceCache::create_pipeline(const PipelineDescriptor& desc) const
  -> Pipeline {
  return mDevice.create_pipeline(desc);
}

void ResourceCache::destroy(const Pipeline handle) const noexcept {
  mDevice.destroy(handle);
}

auto ResourceCache::create_vertex_buffer(
  const VertexBufferDescriptor& desc, const span<const byte> initialData) const
  -> VertexBuffer {
  return mDevice.create_vertex_buffer(desc, initialData);
}

void ResourceCache::destroy(const VertexBuffer handle) const noexcept {
  mDevice.destroy(handle);
}

auto ResourceCache::create_sampler(const SamplerDescriptor& desc) const
  -> Sampler {
  return mDevice.create_sampler(desc);
}

void ResourceCache::destroy(const Sampler handle) const noexcept {
  mDevice.destroy(handle);
}

auto ResourceCache::load_texture(const path& path) const -> Texture {
  return mDevice.load_texture(path);
}

void ResourceCache::destroy(const Texture handle) const noexcept {
  mDevice.destroy(handle);
}

auto ResourceCache::load_x_model(const path& path) const -> ext::XModel {
  const auto modelExt {
    *gfx::query_device_extension<ext::XModelSupport>(mDevice)};

  return modelExt->load(path.u8string());
}

auto ResourceCache::create_mesh(const MeshDescriptor& desc) -> Mesh {
  const VertexBufferDescriptor vbDesc {desc.data.size_bytes(), desc.layout};

  auto [mesh, data] = mMeshes.allocate();
  data.vertexBuffer = mDevice.create_vertex_buffer(vbDesc, desc.data);
  data.vertexCount = desc.vertexCount;

  return mesh;
}

auto ResourceCache::create_material(const MaterialDescriptor& desc)
  -> Material {
  auto [handle, data] = mMaterials.allocate();

  TextureBlendingStage textureStage {};
  textureStage.texCoordinateSrc = desc.textureCoordinateSource;
  textureStage.texCoordinateTransformMode = desc.textureTransformMode;
  textureStage.texCoordinateProjected = desc.textureTransformProjected;

  data.pipeline = mDevice.create_pipeline(PipelineDescriptor {
    span {&textureStage, 1},
    desc.primitiveType,
    desc.lit,
    desc.cullBackFace ? CullMode::CounterClockwise : CullMode::None,
    DepthTestPass::IfLessEqual,
    true,
  });

  data.renderStates[RenderStateType::FillMode] =
    desc.solid ? FillMode::Solid : FillMode::Wireframe;

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

auto ResourceCache::get(const Mesh mesh) const -> const MeshData& {
  return mMeshes[mesh];
}

auto ResourceCache::get(const Material material) const -> const MaterialData& {
  return mMaterials[material];
}

} // namespace basalt::gfx

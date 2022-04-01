#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/utils.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <tuple>
#include <utility>
#include <vector>

using std::byte;
using std::vector;
using std::filesystem::path;

using gsl::span;

namespace basalt::gfx {

ResourceCache::ResourceCache(Device& device) : mDevice {device} {
}

auto ResourceCache::create_pipeline(const PipelineDescriptor& desc) const
  -> Pipeline {
  return mDevice.create_pipeline(desc);
}

auto ResourceCache::destroy(const Pipeline handle) const noexcept -> void {
  mDevice.destroy(handle);
}

auto ResourceCache::create_vertex_buffer(
  const VertexBufferDescriptor& desc, const span<const byte> initialData) const
  -> VertexBuffer {
  return mDevice.create_vertex_buffer(desc, initialData);
}

auto ResourceCache::destroy(const VertexBuffer handle) const noexcept -> void {
  mDevice.destroy(handle);
}

auto ResourceCache::create_sampler(const SamplerDescriptor& desc) const
  -> Sampler {
  return mDevice.create_sampler(desc);
}

auto ResourceCache::destroy(const Sampler handle) const noexcept -> void {
  mDevice.destroy(handle);
}

auto ResourceCache::load_texture(const path& path) const -> Texture {
  return mDevice.load_texture(path);
}

auto ResourceCache::destroy(const Texture handle) const noexcept -> void {
  mDevice.destroy(handle);
}

auto ResourceCache::load_x_model(const path& path) -> ext::XModel {
  const auto modelExt {
    *gfx::query_device_extension<ext::XModelSupport>(mDevice)};
  BASALT_ASSERT(modelExt, "X model files not supported");

  const ext::XModelData xModel {modelExt->load(path)};

  vector<Material> materials;
  materials.reserve(xModel.materials().size());

  for (const auto& material : xModel.materials()) {
    MaterialDescriptor desc;
    desc.diffuse = material.diffuse;
    desc.ambient = material.ambient;
    desc.sampledTexture.texture = load_texture(material.textureFile);

    materials.push_back(create_material(desc));
  }

  return std::get<ext::XModel>(
    mXModels.allocate(XModelData {std::move(materials), xModel.mesh()}));
}

auto ResourceCache::get(const ext::XModel handle) const -> const XModelData& {
  return mXModels[handle];
}

auto ResourceCache::destroy(const ext::XModel handle) noexcept -> void {
  if (!mXModels.is_handle_valid(handle)) {
    return;
  }

  {
    const auto& data {get(handle)};

    for (const Material material : data.materials) {
      destroy(get(material).texture);
      destroy(material);
    }

    const auto modelExt {
      *gfx::query_device_extension<ext::XModelSupport>(mDevice)};
    modelExt->destroy(data.mesh);
  }

  mXModels.deallocate(handle);
}

auto ResourceCache::create_mesh(const MeshDescriptor& desc) -> Mesh {
  const VertexBuffer vb {create_vertex_buffer(
    VertexBufferDescriptor {
      desc.vertexData.size_bytes(),
      desc.layout,
    },
    desc.vertexData)};

  return std::get<Mesh>(mMeshes.allocate(vb, 0u, desc.vertexCount));
}

auto ResourceCache::get(const Mesh handle) const -> const MeshData& {
  return mMeshes[handle];
}

auto ResourceCache::destroy(const Mesh handle) noexcept -> void {
  if (!mMeshes.is_handle_valid(handle)) {
    return;
  }

  {
    auto& data {get(handle)};
    destroy(data.vertexBuffer);
  }

  mMeshes.deallocate(handle);
}

auto ResourceCache::create_material(const MaterialDescriptor& desc)
  -> Material {
  TextureBlendingStage textureStage;
  textureStage.texCoordinateSrc = desc.textureCoordinateSource;
  textureStage.texCoordinateTransformMode = desc.textureTransformMode;
  textureStage.texCoordinateProjected = desc.textureTransformProjected;

  const Pipeline pipeline {create_pipeline(PipelineDescriptor {
    span {&textureStage, 1},
    desc.primitiveType,
    desc.lit,
    desc.cullBackFace ? CullMode::CounterClockwise : CullMode::None,
    DepthTestPass::IfLessEqual,
    true,
  })};

  // TODO: cache samplers
  const Sampler sampler {create_sampler(SamplerDescriptor {
    desc.sampledTexture.filter, desc.sampledTexture.mipFilter,
    desc.sampledTexture.addressModeU, desc.sampledTexture.addressModeV})};

  return std::get<Material>(mMaterials.allocate(MaterialData {
    MaterialData::RenderStates {
      {RenderStateType::FillMode,
       desc.solid ? FillMode::Solid : FillMode::Wireframe},
    },
    desc.diffuse,
    desc.ambient,
    pipeline,
    desc.sampledTexture.texture,
    sampler,
  }));
}

auto ResourceCache::get(const Material material) const -> const MaterialData& {
  return mMaterials[material];
}

auto ResourceCache::destroy(const Material handle) noexcept -> void {
  {
    const auto& data {get(handle)};

    destroy(data.sampler);
    destroy(data.pipeline);
  }

  mMaterials.deallocate(handle);
}

} // namespace basalt::gfx

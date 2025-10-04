#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <gsl/span>

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <iterator>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

using std::byte;
using std::vector;
using std::filesystem::path;

using gsl::span;

namespace basalt::gfx {

auto ResourceCache::create(ContextPtr context) -> ResourceCachePtr {
  return std::make_shared<ResourceCache>(std::move(context));
}

ResourceCache::ResourceCache(ContextPtr context)
  : mContext{std::move(context)} {
}

ResourceCache::~ResourceCache() noexcept {
  destroy_all();
}

auto ResourceCache::context() const noexcept -> ContextPtr const& {
  return mContext;
}

auto ResourceCache::create_material(MaterialCreateInfo const& createInfo)
  -> MaterialHandle {
  auto const handle = mContext->create_material(createInfo).release();
  mMaterials.push_back(handle);

  return handle;
}

auto ResourceCache::create_material_class(MaterialClassCreateInfo const& info)
  -> MaterialClassHandle {
  auto const handle = mContext->create_material_class(info).release();
  mMaterialClasses.push_back(handle);

  return handle;
}

auto ResourceCache::create_pipeline(PipelineCreateInfo const& desc)
  -> PipelineHandle {
  auto const handle = mContext->create_pipeline(desc).release();
  mPipelines.push_back(handle);

  return handle;
}

auto ResourceCache::create_sampler(SamplerCreateInfo const& desc)
  -> SamplerHandle {
  auto const handle = mContext->create_sampler(desc).release();
  mSamplers.push_back(handle);

  return handle;
}

auto ResourceCache::load_texture_2d(path const& filePath) -> TextureHandle {
  auto const handle = mContext->load_texture_2d(filePath).release();
  mTextures.push_back(handle);

  return handle;
}

auto ResourceCache::load_texture_cube(path const& filePath) -> TextureHandle {
  auto const handle = mContext->load_texture_cube(filePath).release();
  mTextures.push_back(handle);

  return handle;
}

auto ResourceCache::load_texture_3d(path const& filePath) -> TextureHandle {
  auto const handle = mContext->load_texture_3d(filePath).release();
  mTextures.push_back(handle);

  return handle;
}

auto ResourceCache::compile_effect(path const& filePath) -> ext::CompileResult {
  auto result = mContext->compile_effect(filePath);
  if (auto const* handle = std::get_if<ext::EffectId>(&result)) {
    mEffects.push_back(*handle);
  }

  return result;
}

auto ResourceCache::create_vertex_buffer(VertexBufferCreateInfo const& desc,
                                         span<byte const> const initialData)
  -> VertexBufferHandle {
  auto const handle =
    mContext->create_vertex_buffer(desc, initialData).release();
  mVertexBuffers.push_back(handle);

  return handle;
}

auto ResourceCache::create_index_buffer(IndexBufferCreateInfo const& desc,
                                        span<byte const> const initialData)
  -> IndexBufferHandle {
  auto const handle =
    mContext->create_index_buffer(desc, initialData).release();
  mIndexBuffers.push_back(handle);

  return handle;
}

auto ResourceCache::create_mesh(MeshCreateInfo const& createInfo)
  -> MeshHandle {
  auto const meshHandle = mContext->create_mesh(createInfo).release();
  mMeshes.push_back(meshHandle);

  return meshHandle;
}

auto ResourceCache::load_x_meshes(path const& filePath) -> ext::XModelData {
  auto data = mContext->load_x_meshes(filePath);

  std::copy(data.meshes.begin(), data.meshes.end(),
            std::back_inserter(mXMeshes));

  return data;
}

auto ResourceCache::destroy_all() noexcept -> void {
  for (auto const handle : mXMeshes) {
    mContext->destroy(handle);
  }

  for (auto const handle : mMeshes) {
    mContext->destroy(handle);
  }

  for (auto const handle : mIndexBuffers) {
    mContext->destroy(handle);
  }

  for (auto const handle : mVertexBuffers) {
    mContext->destroy(handle);
  }

  for (auto const handle : mEffects) {
    mContext->destroy(handle);
  }

  for (auto const handle : mMaterials) {
    mContext->destroy(handle);
  }

  for (auto const handle : mMaterialClasses) {
    mContext->destroy(handle);
  }

  for (auto const handle : mTextures) {
    mContext->destroy(handle);
  }

  for (auto const handle : mSamplers) {
    mContext->destroy(handle);
  }

  for (auto const handle : mPipelines) {
    mContext->destroy(handle);
  }
}

} // namespace basalt::gfx

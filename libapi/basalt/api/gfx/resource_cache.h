#pragma once

#include "types.h"

#include "backend/types.h"
#include "backend/ext/types.h"
#include "backend/ext/x_model_support.h"

#include <gsl/span>

#include <cstddef>
#include <filesystem>
#include <vector>

namespace basalt::gfx {

class ResourceCache final {
public:
  static auto create(ContextPtr) -> ResourceCachePtr;

  // don't use. Call create() instead
  explicit ResourceCache(ContextPtr);

  ResourceCache(ResourceCache const&) = delete;

  ResourceCache(ResourceCache&&) = delete;

  ~ResourceCache() noexcept;

  auto operator=(ResourceCache const&) -> ResourceCache& = delete;

  auto operator=(ResourceCache&&) -> ResourceCache& = delete;

  [[nodiscard]]
  auto context() const noexcept -> ContextPtr const&;

  [[nodiscard]]
  auto create_pipeline(PipelineCreateInfo const&) -> PipelineHandle;

  [[nodiscard]]
  auto create_sampler(SamplerCreateInfo const&) -> SamplerHandle;

  [[nodiscard]]
  auto load_texture_2d(std::filesystem::path const&) -> TextureHandle;

  [[nodiscard]]
  auto load_texture_cube(std::filesystem::path const&) -> TextureHandle;

  [[nodiscard]]
  auto load_texture_3d(std::filesystem::path const&) -> TextureHandle;

  [[nodiscard]]
  auto create_material(MaterialCreateInfo const&) -> MaterialHandle;

  [[nodiscard]]
  auto compile_effect(std::filesystem::path const&) -> ext::CompileResult;

  [[nodiscard]]
  auto create_vertex_buffer(VertexBufferCreateInfo const&,
                            gsl::span<std::byte const> initialData = {})
    -> VertexBufferHandle;

  [[nodiscard]]
  auto create_index_buffer(IndexBufferCreateInfo const&,
                           gsl::span<std::byte const> initialData = {})
    -> IndexBufferHandle;

  [[nodiscard]]
  auto create_mesh(MeshCreateInfo const&) -> MeshHandle;

  [[nodiscard]]
  auto load_x_meshes(std::filesystem::path const&) -> ext::XModelData;

private:
  ContextPtr mContext;

  // owned resources
  std::vector<PipelineHandle> mPipelines;
  std::vector<SamplerHandle> mSamplers;
  std::vector<TextureHandle> mTextures;
  std::vector<MaterialHandle> mMaterials;
  std::vector<ext::EffectId> mEffects;
  std::vector<VertexBufferHandle> mVertexBuffers;
  std::vector<IndexBufferHandle> mIndexBuffers;
  std::vector<MeshHandle> mMeshes;
  std::vector<ext::XMeshHandle> mXMeshes;
};

} // namespace basalt::gfx

#pragma once

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/device.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/handle_pool.h>
#include <basalt/api/shared/types.h>

#include <gsl/span>

#include <cstddef>
#include <filesystem>
#include <unordered_map>
#include <utility>

namespace basalt::gfx {

struct ResourceCache {
  explicit ResourceCache(ResourceRegistryPtr, Device&);

  template <typename T>
  auto load(ResourceId) -> T;

  [[nodiscard]] auto is_loaded(ResourceId) const -> bool;

  template <typename T>
  auto get(ResourceId) -> T;

  [[nodiscard]] auto create_pipeline(const PipelineDescriptor&) const
    -> Pipeline;
  void destroy(Pipeline) const noexcept;

  [[nodiscard]] auto
  create_vertex_buffer(const VertexBufferDescriptor&,
                       gsl::span<const std::byte> initialData = {}) const
    -> VertexBuffer;

  void destroy(VertexBuffer) const noexcept;

  // span is empty on failure
  // F = void(gsl::span<std::byte>)
  template <typename F>
  void with_mapping_of(const VertexBuffer handle, F&& func) {
    with_mapping_of(handle, 0ull, 0ull, std::forward<F>(func));
  }

  // offset = 0 && size = 0 maps entire buffer
  // size = 0 maps from offset until the end of the buffer
  // span is empty on failure
  // F = void(gsl::span<std::byte>)
  template <typename F>
  void with_mapping_of(const VertexBuffer handle, const uDeviceSize offset,
                       const uDeviceSize size, F&& func) {
    // TODO: how should this handle map failure? right now its passing the empty
    // span to the function
    func(mDevice.map(handle, offset, size));

    mDevice.unmap(handle);
  }

  [[nodiscard]] auto create_sampler(const SamplerDescriptor&) const -> Sampler;
  void destroy(Sampler) const noexcept;

  [[nodiscard]] auto load_texture(const std::filesystem::path&) const
    -> Texture;
  void destroy(Texture) const noexcept;

  auto create_mesh(const MeshDescriptor&) -> Mesh;
  auto create_material(const MaterialDescriptor&) -> Material;

  [[nodiscard]] auto get(Mesh) const -> const MeshData&;
  [[nodiscard]] auto get(Material) const -> const MaterialData&;

private:
  ResourceRegistryPtr mResourceRegistry;
  Device& mDevice;
  std::unordered_map<ResourceId, ext::XModel> mModels;
  std::unordered_map<ResourceId, Texture> mTextures;
  HandlePool<MeshData, Mesh> mMeshes;
  HandlePool<MaterialData, Material> mMaterials;
};

template <>
[[nodiscard]] auto ResourceCache::load(ResourceId) -> ext::XModel;
template <>
[[nodiscard]] auto ResourceCache::load(ResourceId) -> Texture;

template <>
[[nodiscard]] auto ResourceCache::get(ResourceId) -> ext::XModel;
template <>
[[nodiscard]] auto ResourceCache::get(ResourceId) -> Texture;

} // namespace basalt::gfx

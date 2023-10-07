#pragma once

#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/handle_pool.h>

#include <gsl/span>

#include <cstddef>
#include <filesystem>
#include <utility>
#include <vector>

namespace basalt::gfx {

class ResourceCache final {
public:
  static auto create(DevicePtr) -> ResourceCachePtr;

  ResourceCache(const ResourceCache&) = delete;
  ResourceCache(ResourceCache&&) = delete;

  ~ResourceCache() noexcept;

  auto operator=(const ResourceCache&) -> ResourceCache& = delete;
  auto operator=(ResourceCache&&) -> ResourceCache& = delete;

  [[nodiscard]] auto create_pipeline(const PipelineDescriptor&) -> Pipeline;
  auto destroy(Pipeline) noexcept -> void;

  [[nodiscard]] auto
  create_vertex_buffer(const VertexBufferDescriptor&,
                       gsl::span<const std::byte> initialData = {})
    -> VertexBuffer;
  auto destroy(VertexBuffer) noexcept -> void;

  // span is empty on failure
  // F = void(gsl::span<std::byte>)
  template <typename F>
  auto with_mapping_of(const VertexBuffer handle, F&& func) -> void {
    with_mapping_of(handle, 0ull, 0ull, std::forward<F>(func));
  }

  // offset = 0 && size = 0 maps entire buffer
  // size = 0 maps from offset until the end of the buffer
  // span is empty on failure
  // F = void(gsl::span<std::byte>)
  template <typename F>
  auto with_mapping_of(const VertexBuffer handle, const uDeviceSize offset,
                       const uDeviceSize size, F&& func) -> void {
    // TODO: how should this handle map failure? right now its passing the empty
    // span to the function
    func(map(handle, offset, size));

    unmap(handle);
  }

  [[nodiscard]] auto
  create_index_buffer(const IndexBufferDescriptor&,
                      gsl::span<const std::byte> initialData = {})
    -> IndexBuffer;
  auto destroy(IndexBuffer) noexcept -> void;

  // span is empty on failure
  // F = void(gsl::span<std::byte>)
  template <typename F>
  auto with_mapping_of(const IndexBuffer handle, F&& func) -> void {
    with_mapping_of(handle, 0ull, 0ull, std::forward<F>(func));
  }

  // offsetInBytes = 0 && sizeInBytes = 0 maps entire buffer
  // sizeInBytes = 0 maps from offsetInBytes until the end of the buffer
  // span is empty on failure
  // F = void(gsl::span<std::byte>)
  template <typename F>
  auto with_mapping_of(const IndexBuffer handle,
                       const uDeviceSize offsetInBytes,
                       const uDeviceSize sizeInBytes, F&& func) -> void {
    // TODO: how should this handle map failure? right now its passing the empty
    // span to the function
    func(map(handle, offsetInBytes, sizeInBytes));

    unmap(handle);
  }

  [[nodiscard]] auto create_sampler(const SamplerDescriptor&) -> Sampler;
  auto destroy(Sampler) noexcept -> void;

  [[nodiscard]] auto load_texture(const std::filesystem::path&) -> Texture;
  [[nodiscard]] auto load_cube_texture(const std::filesystem::path&) -> Texture;
  auto destroy(Texture) noexcept -> void;

  auto load_texture_3d(const std::filesystem::path&) -> Texture;

  [[nodiscard]] auto load_x_model(const std::filesystem::path&) -> ext::XModel;
  [[nodiscard]] auto load_x_model(const XModelDescriptor&) -> ext::XModel;
  auto destroy(ext::XModel) noexcept -> void;
  [[nodiscard]] auto get(ext::XModel) const -> const XModelData&;

  [[nodiscard]] auto create_mesh(const MeshDescriptor&) -> Mesh;
  auto destroy(Mesh) noexcept -> void;
  [[nodiscard]] auto get(Mesh) const -> const MeshData&;

  [[nodiscard]] auto create_material(const MaterialDescriptor&) -> Material;
  auto destroy(Material) noexcept -> void;
  [[nodiscard]] auto get(Material) const -> const MaterialData&;

  [[nodiscard]] auto compile_effect(std::filesystem::path const&)
    -> ext::CompileResult;
  auto destroy(ext::EffectId) noexcept -> void;
  [[nodiscard]] auto get(ext::EffectId) const -> ext::Effect&;

  explicit ResourceCache(DevicePtr);

private:
  DevicePtr mDevice;

  // owned resources
  std::vector<Pipeline> mPipelines;
  std::vector<VertexBuffer> mVertexBuffers;
  std::vector<IndexBuffer> mIndexBuffers;
  std::vector<Sampler> mSamplers;
  std::vector<Texture> mTextures;
  HandlePool<MeshData, Mesh> mMeshes;
  HandlePool<MaterialData, Material> mMaterials;
  HandlePool<XModelData, ext::XModel> mXModels;
  std::vector<ext::EffectId> mEffects;

  [[nodiscard]] auto map(VertexBuffer, uDeviceSize offset = 0,
                         uDeviceSize size = 0) const -> gsl::span<std::byte>;
  auto unmap(VertexBuffer) const -> void;
  [[nodiscard]] auto map(IndexBuffer, uDeviceSize offsetInBytes = 0,
                         uDeviceSize sizeInBytes = 0) const
    -> gsl::span<std::byte>;
  auto unmap(IndexBuffer) const -> void;

  auto destroy_data(ext::XModel) noexcept -> void;
  auto destroy_data(Mesh) noexcept -> void;
  auto destroy_data(Material) noexcept -> void;
};

} // namespace basalt::gfx

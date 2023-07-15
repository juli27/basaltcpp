#pragma once

#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/handle_pool.h>

#include <gsl/span>

#include <cstddef>
#include <filesystem>
#include <utility>

namespace basalt::gfx {

class ResourceCache final {
public:
  static auto create(DevicePtr) -> ResourceCachePtr;
  
  [[nodiscard]] auto create_pipeline(const PipelineDescriptor&) const
    -> Pipeline;
  auto destroy(Pipeline) const noexcept -> void;

  [[nodiscard]] auto
  create_vertex_buffer(const VertexBufferDescriptor&,
                       gsl::span<const std::byte> initialData = {}) const
    -> VertexBuffer;

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

  auto destroy(VertexBuffer) const noexcept -> void;

  [[nodiscard]] auto
  create_index_buffer(const IndexBufferDescriptor&,
                      gsl::span<const std::byte> initialData = {}) const
    -> IndexBuffer;

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

  auto destroy(IndexBuffer) const noexcept -> void;

  [[nodiscard]] auto create_sampler(const SamplerDescriptor&) const -> Sampler;
  auto destroy(Sampler) const noexcept -> void;

  [[nodiscard]] auto load_texture(const std::filesystem::path&) const
    -> Texture;
  auto destroy(Texture) const noexcept -> void;

  [[nodiscard]] auto load_x_model(const std::filesystem::path&) -> ext::XModel;
  [[nodiscard]] auto get(ext::XModel) const -> const XModelData&;
  auto destroy(ext::XModel) noexcept -> void;

  [[nodiscard]] auto create_mesh(const MeshDescriptor&) -> Mesh;
  [[nodiscard]] auto get(Mesh) const -> const MeshData&;
  auto destroy(Mesh) noexcept -> void;

  [[nodiscard]] auto create_material(const MaterialDescriptor&) -> Material;
  [[nodiscard]] auto get(Material) const -> const MaterialData&;
  auto destroy(Material) noexcept -> void;

  explicit ResourceCache(DevicePtr); 

private:
  DevicePtr mDevice;
  HandlePool<MeshData, Mesh> mMeshes;
  HandlePool<MaterialData, Material> mMaterials;
  HandlePool<XModelData, ext::XModel> mXModels;

  [[nodiscard]] auto map(VertexBuffer, uDeviceSize offset = 0,
                         uDeviceSize size = 0) const -> gsl::span<std::byte>;
  auto unmap(VertexBuffer) const -> void;
  [[nodiscard]] auto map(IndexBuffer, uDeviceSize offsetInBytes = 0,
                         uDeviceSize sizeInBytes = 0) const
    -> gsl::span<std::byte>;
  auto unmap(IndexBuffer) const -> void;
};

} // namespace basalt::gfx

#pragma once

#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/device.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/handle_pool.h>

#include <gsl/span>

#include <cstddef>
#include <filesystem>
#include <utility>

namespace basalt::gfx {

struct ResourceCache {
  explicit ResourceCache(DevicePtr);

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
    func(mDevice->map(handle, offset, size));

    mDevice->unmap(handle);
  }

  auto destroy(VertexBuffer) const noexcept -> void;

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

private:
  DevicePtr mDevice;
  HandlePool<MeshData, Mesh> mMeshes;
  HandlePool<MaterialData, Material> mMaterials;
  HandlePool<XModelData, ext::XModel> mXModels;
};

} // namespace basalt::gfx

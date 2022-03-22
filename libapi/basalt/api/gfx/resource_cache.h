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
  explicit ResourceCache(Device&);

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

  [[nodiscard]] auto load_x_model(const std::filesystem::path&) const
    -> ext::XModel;

  auto create_mesh(const MeshDescriptor&) -> Mesh;
  auto create_material(const MaterialDescriptor&) -> Material;

  [[nodiscard]] auto get(Mesh) const -> const MeshData&;
  [[nodiscard]] auto get(Material) const -> const MaterialData&;

private:
  Device& mDevice;
  HandlePool<MeshData, Mesh> mMeshes;
  HandlePool<MaterialData, Material> mMaterials;
};

} // namespace basalt::gfx

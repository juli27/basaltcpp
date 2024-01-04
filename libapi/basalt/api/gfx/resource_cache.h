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
  static auto create(ContextPtr) -> ResourceCachePtr;

  // don't use. Call create() instead
  explicit ResourceCache(ContextPtr);

  ResourceCache(ResourceCache const&) = delete;

  ResourceCache(ResourceCache&&) = delete;

  ~ResourceCache() noexcept;

  auto operator=(ResourceCache const&) -> ResourceCache& = delete;

  auto operator=(ResourceCache&&) -> ResourceCache& = delete;

  [[nodiscard]] auto create_pipeline(PipelineDescriptor const&) -> Pipeline;
  auto destroy(Pipeline) noexcept -> void;

  [[nodiscard]] auto
  create_vertex_buffer(VertexBufferDescriptor const&,
                       gsl::span<std::byte const> initialData = {})
    -> VertexBuffer;
  auto destroy(VertexBuffer) noexcept -> void;

  // span is empty on failure
  // F = void(gsl::span<std::byte>)
  template <typename F>
  auto with_mapping_of(VertexBuffer const handle, F&& func) -> void {
    with_mapping_of(handle, 0ull, 0ull, std::forward<F>(func));
  }

  // offset = 0 && size = 0 maps entire buffer
  // size = 0 maps from offset until the end of the buffer
  // span is empty on failure
  // F = void(gsl::span<std::byte>)
  template <typename F>
  auto with_mapping_of(VertexBuffer const handle, uDeviceSize const offset,
                       uDeviceSize const size, F&& func) -> void {
    // TODO: how should this handle map failure? right now its passing the empty
    // span to the function
    func(map(handle, offset, size));

    unmap(handle);
  }

  [[nodiscard]] auto
  create_index_buffer(IndexBufferDescriptor const&,
                      gsl::span<std::byte const> initialData = {})
    -> IndexBuffer;
  auto destroy(IndexBuffer) noexcept -> void;

  // span is empty on failure
  // F = void(gsl::span<std::byte>)
  template <typename F>
  auto with_mapping_of(IndexBuffer const handle, F&& func) -> void {
    with_mapping_of(handle, 0ull, 0ull, std::forward<F>(func));
  }

  // offsetInBytes = 0 && sizeInBytes = 0 maps entire buffer
  // sizeInBytes = 0 maps from offsetInBytes until the end of the buffer
  // span is empty on failure
  // F = void(gsl::span<std::byte>)
  template <typename F>
  auto with_mapping_of(IndexBuffer const handle,
                       uDeviceSize const offsetInBytes,
                       uDeviceSize const sizeInBytes, F&& func) -> void {
    // TODO: how should this handle map failure? right now its passing the empty
    // span to the function
    func(map(handle, offsetInBytes, sizeInBytes));

    unmap(handle);
  }

  [[nodiscard]] auto create_sampler(SamplerDescriptor const&) -> Sampler;
  auto destroy(Sampler) noexcept -> void;

  [[nodiscard]] auto load_texture(std::filesystem::path const&) -> Texture;
  [[nodiscard]] auto load_cube_texture(std::filesystem::path const&) -> Texture;
  auto destroy(Texture) noexcept -> void;

  auto load_texture_3d(std::filesystem::path const&) -> Texture;

  [[nodiscard]] auto load_x_model(std::filesystem::path const&) -> ext::XModel;
  [[nodiscard]] auto load_x_model(XModelDescriptor const&) -> ext::XModel;
  auto destroy(ext::XModel) noexcept -> void;
  [[nodiscard]] auto get(ext::XModel) const -> XModelData const&;

  [[nodiscard]] auto create_mesh(MeshDescriptor const&) -> Mesh;
  auto destroy(Mesh) noexcept -> void;
  [[nodiscard]] auto get(Mesh) const -> MeshData const&;

  [[nodiscard]] auto create_material(MaterialDescriptor const&) -> Material;
  auto destroy(Material) noexcept -> void;
  [[nodiscard]] auto get(Material) const -> MaterialData const&;

  [[nodiscard]] auto compile_effect(std::filesystem::path const&)
    -> ext::CompileResult;
  auto destroy(ext::EffectId) noexcept -> void;
  [[nodiscard]] auto get(ext::EffectId) const -> ext::Effect&;

private:
  ContextPtr mContext;
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
};

} // namespace basalt::gfx

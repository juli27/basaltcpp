#pragma once

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/handle_pool.h>

#include <gsl/span>

#include <cstddef>
#include <filesystem>
#include <memory>
#include <optional>
#include <utility>

namespace basalt::gfx {

struct ContextCreateInfo {
  [[nodiscard]]
  static auto create_default(AdapterList const&) -> ContextCreateInfo;

  Adapter adapter;
  DisplayMode exclusiveDisplayMode;
  ImageFormat renderTargetFormat;
  ImageFormat depthStencilFormat;
  MultiSampleCount sampleCount;
};

class Context final : public std::enable_shared_from_this<Context> {
public:
  static auto create(DevicePtr, ext::DeviceExtensions, SwapChainPtr, Info)
    -> ContextPtr;

  // don't use. Use create() function instead
  Context(DevicePtr, ext::DeviceExtensions, SwapChainPtr, Info);

  Context(Context const&) = delete;

  Context(Context&&) = delete;

  ~Context() noexcept = default;

  auto operator=(Context const&) -> Context& = delete;

  auto operator=(Context&&) -> Context& = delete;

  [[nodiscard]]
  auto gfx_info() const noexcept -> Info const&;

  [[nodiscard]]
  auto create_resource_cache() -> ResourceCachePtr;

  [[nodiscard]]
  auto create_pipeline(PipelineCreateInfo const&) const -> PipelineHandle;

  auto destroy(PipelineHandle) const noexcept -> void;

  [[nodiscard]]
  auto create_sampler(SamplerCreateInfo const&) const -> SamplerHandle;

  auto destroy(SamplerHandle) const noexcept -> void;

  [[nodiscard]]
  auto load_texture_2d(std::filesystem::path const&) const -> TextureHandle;

  [[nodiscard]]
  auto load_texture_cube(std::filesystem::path const&) const -> TextureHandle;

  [[nodiscard]]
  auto load_texture_3d(std::filesystem::path const&) const -> TextureHandle;

  auto destroy(TextureHandle) const noexcept -> void;

  [[nodiscard]]
  auto create_material(MaterialCreateInfo const&) -> MaterialHandle;

  auto destroy(MaterialHandle) noexcept -> void;

  [[nodiscard]] auto get(MaterialHandle) const -> MaterialData const&;

  [[nodiscard]]
  auto compile_effect(std::filesystem::path const&) const -> ext::CompileResult;

  auto destroy(ext::EffectId) const noexcept -> void;

  [[nodiscard]] auto get(ext::EffectId) const -> ext::Effect&;

  [[nodiscard]]
  auto create_vertex_buffer(VertexBufferCreateInfo const&,
                            gsl::span<std::byte const> initialData = {}) const
    -> VertexBufferHandle;

  auto destroy(VertexBufferHandle) const noexcept -> void;

  // span is empty on failure
  // F = void(gsl::span<std::byte>)
  template <typename F>
  auto with_mapping_of(VertexBufferHandle const vb, F&& func) const -> void {
    with_mapping_of(vb, 0, 0, std::forward<F>(func));
  }

  // offset = 0 && sizeInBytes = 0 maps entire buffer
  // sizeInBytes = 0 maps from offset until the end of the buffer
  // span is empty on failure
  // F = void(gsl::span<std::byte>)
  template <typename F>
  auto with_mapping_of(VertexBufferHandle const vb, uDeviceSize const offset,
                       uDeviceSize const sizeInBytes, F&& func) const -> void {
    // TODO: how should this handle map failure? right now it's passing the
    // empty span to the function
    std::forward<F>(func)(map(vb, offset, sizeInBytes));

    unmap(vb);
  }

  [[nodiscard]]
  auto create_index_buffer(IndexBufferCreateInfo const&,
                           gsl::span<std::byte const> initialData = {}) const
    -> IndexBufferHandle;

  auto destroy(IndexBufferHandle) const noexcept -> void;

  // span is empty on failure
  // F = void(gsl::span<std::byte>)
  template <typename F>
  auto with_mapping_of(IndexBufferHandle const ib, F&& func) const -> void {
    with_mapping_of(ib, 0, 0, std::forward<F>(func));
  }

  // offsetInBytes = 0 && sizeInBytes = 0 maps entire buffer
  // sizeInBytes = 0 maps from offsetInBytes until the end of the buffer
  // span is empty on failure
  // F = void(gsl::span<std::byte>)
  template <typename F>
  auto with_mapping_of(IndexBufferHandle const ib,
                       uDeviceSize const offsetInBytes,
                       uDeviceSize const sizeInBytes, F&& func) const -> void {
    // TODO: how should this handle map failure? right now it's passing the
    // empty span to the function
    std::forward<F>(func)(map(ib, offsetInBytes, sizeInBytes));

    unmap(ib);
  }

  [[nodiscard]]
  auto create_mesh(MeshCreateInfo const&) -> MeshHandle;

  auto destroy(MeshHandle) noexcept -> void;

  [[nodiscard]] auto get(MeshHandle) const -> MeshData const&;

  [[nodiscard]]
  auto load_x_model(XModelLoadInfo const&) -> ext::XModelHandle;

  [[nodiscard]]
  auto create_x_model(ext::XModelCreateInfo const&) -> ext::XModelHandle;

  auto destroy(ext::XModelHandle) noexcept -> void;

  auto get(ext::XModelHandle) const -> XModelData const&;

  auto submit(gsl::span<CommandList const>) const -> void;

  // engine-private
  [[nodiscard]]
  auto device() const noexcept -> DevicePtr const&;
  [[nodiscard]]
  auto swap_chain() const noexcept -> SwapChainPtr const&;

  template <typename T>
  [[nodiscard]]
  auto query_device_extension() const -> std::optional<std::shared_ptr<T>> {
    static_assert(std::is_base_of_v<ext::DeviceExtension, T>);

    if (auto const maybeExt = query_device_extension(T::ID)) {
      return std::static_pointer_cast<T>(*maybeExt);
    }

    return std::nullopt;
  }

private:
  DevicePtr mDevice;
  ext::DeviceExtensions mDeviceExtensions;
  SwapChainPtr mSwapChain;
  Info mInfo;
  HandlePool<MaterialData, MaterialHandle> mMaterials;
  HandlePool<MeshData, MeshHandle> mMeshes;
  HandlePool<XModelData, ext::XModelHandle> mXModels;

  auto query_device_extension(ext::DeviceExtensionId) const
    -> std::optional<ext::DeviceExtensionPtr>;

  [[nodiscard]]
  auto map(VertexBufferHandle, uDeviceSize offsetInBytes = 0,
           uDeviceSize sizeInBytes = 0) const -> gsl::span<std::byte>;

  auto unmap(VertexBufferHandle) const -> void;

  [[nodiscard]]
  auto map(IndexBufferHandle, uDeviceSize offsetInBytes = 0,
           uDeviceSize sizeInBytes = 0) const -> gsl::span<std::byte>;

  auto unmap(IndexBufferHandle) const -> void;
};

} // namespace basalt::gfx

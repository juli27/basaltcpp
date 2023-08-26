#pragma once

#include <basalt/gfx/backend/device.h>

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>
#include <basalt/gfx/backend/d3d9/data.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/handle_pool.h>

#include <wrl/client.h>

#include <filesystem>
#include <memory>
#include <unordered_map>

namespace basalt::gfx {

class D3D9Device final : public Device {
public:
  explicit D3D9Device(Microsoft::WRL::ComPtr<IDirect3DDevice9> device);

  auto reset(D3DPRESENT_PARAMETERS&) const -> void;

  auto execute(const CommandList&) -> void;

  [[nodiscard]] auto load_texture_3d(const std::filesystem::path&) -> Texture;

  [[nodiscard]] auto capabilities() const -> const DeviceCaps& override;
  [[nodiscard]] auto get_status() const noexcept -> DeviceStatus override;

  auto reset() -> void override;

  [[nodiscard]] auto create_pipeline(const PipelineDescriptor&)
    -> Pipeline override;

  auto destroy(Pipeline) noexcept -> void override;

  [[nodiscard]] auto
  create_vertex_buffer(const VertexBufferDescriptor&,
                       gsl::span<const std::byte> initialData)
    -> VertexBuffer override;

  auto destroy(VertexBuffer) noexcept -> void override;

  [[nodiscard]] auto map(VertexBuffer, uDeviceSize offset, uDeviceSize size)
    -> gsl::span<std::byte> override;
  auto unmap(VertexBuffer) noexcept -> void override;

  [[nodiscard]] auto create_index_buffer(const IndexBufferDescriptor&,
                                         gsl::span<const std::byte> initialData)
    -> IndexBuffer override;

  auto destroy(IndexBuffer) noexcept -> void override;

  [[nodiscard]] auto map(IndexBuffer, uDeviceSize offsetInBytes,
                         uDeviceSize sizeInBytes)
    -> gsl::span<std::byte> override;

  auto unmap(IndexBuffer) noexcept -> void override;

  [[nodiscard]] auto load_texture(const std::filesystem::path&)
    -> Texture override;

  auto destroy(Texture) noexcept -> void override;

  [[nodiscard]] auto create_sampler(const SamplerDescriptor&)
    -> Sampler override;

  auto destroy(Sampler) noexcept -> void override;

  auto submit(gsl::span<const CommandList>) -> void override;

  auto query_extension(ext::ExtensionId)
    -> std::optional<ext::ExtensionPtr> override;

private:
  using D3D9DevicePtr = Microsoft::WRL::ComPtr<IDirect3DDevice9>;
  using ExtensionMap = std::unordered_map<ext::ExtensionId, ext::ExtensionPtr>;
  using D3D9VertexBufferPtr = Microsoft::WRL::ComPtr<IDirect3DVertexBuffer9>;
  using D3D9IndexBufferPtr = Microsoft::WRL::ComPtr<IDirect3DIndexBuffer9>;
  using D3D9BaseTexturePtr = Microsoft::WRL::ComPtr<IDirect3DBaseTexture9>;

  struct SamplerData final {
    D3DTEXTUREFILTERTYPE magFilter {D3DTEXF_POINT};
    D3DTEXTUREFILTERTYPE minFilter {D3DTEXF_POINT};
    D3DTEXTUREFILTERTYPE mipFilter {D3DTEXF_NONE};
    D3DTEXTUREADDRESS addressModeU {D3DTADDRESS_WRAP};
    D3DTEXTUREADDRESS addressModeV {D3DTADDRESS_WRAP};
    D3DTEXTUREADDRESS addressModeW {D3DTADDRESS_WRAP};
    D3DCOLOR borderColor {0};
    DWORD maxAnisotropy {1};
  };

  D3D9DevicePtr mDevice;

  ExtensionMap mExtensions;

  HandlePool<D3D9Pipeline, Pipeline> mPipelines {};
  HandlePool<D3D9VertexBufferPtr, VertexBuffer> mVertexBuffers {};
  HandlePool<D3D9IndexBufferPtr, IndexBuffer> mIndexBuffers {};
  HandlePool<D3D9BaseTexturePtr, Texture> mTextures {};
  HandlePool<SamplerData, Sampler> mSamplers {};

  DeviceCaps mCaps {};
  D3DPRIMITIVETYPE mCurrentPrimitiveType {D3DPT_POINTLIST};
  u32 mNumLightsUsed {};

  auto execute(const Command&) -> void;
  auto execute(const CommandClearAttachments&) -> void;
  auto execute(const CommandDraw&) -> void;
  auto execute(const CommandDrawIndexed&) -> void;
  auto execute(const CommandBindPipeline&) -> void;
  auto execute(const CommandBindVertexBuffer&) -> void;
  auto execute(const CommandBindIndexBuffer&) -> void;
  auto execute(const CommandBindSampler&) -> void;
  auto execute(const CommandBindTexture&) -> void;
  auto execute(const CommandSetBlendConstant&) -> void;
  auto execute(const CommandSetTransform&) -> void;
  auto execute(const CommandSetAmbientLight&) -> void;
  auto execute(const CommandSetLights&) -> void;
  auto execute(const CommandSetMaterial&) -> void;
  auto execute(const CommandSetFogParameters&) -> void;
  auto execute(const CommandSetReferenceAlpha&) -> void;
  auto execute(const CommandSetTextureFactor&) -> void;
  auto execute(const CommandSetTextureStageConstant&) -> void;

  template <typename T>
  [[nodiscard]] auto get_extension() const -> std::shared_ptr<T> {
    return std::static_pointer_cast<T>(mExtensions.at(T::ID));
  }
};

} // namespace basalt::gfx

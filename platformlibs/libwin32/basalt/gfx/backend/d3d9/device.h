#pragma once

#include <basalt/gfx/backend/device.h>

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>
#include <basalt/gfx/backend/d3d9/data.h>
#include <basalt/gfx/backend/d3d9/types.h>

#include <basalt/gfx/backend/types.h>
#include <basalt/gfx/backend/ext/types.h>

#include <basalt/api/shared/handle_pool.h>

#include <filesystem>
#include <memory>

namespace basalt::gfx {

class D3D9Device final : public Device {
public:
  static auto create(IDirect3DDevice9Ptr, DeviceCaps const&) -> D3D9DevicePtr;

  explicit D3D9Device(IDirect3DDevice9Ptr, DeviceCaps const&);

  [[nodiscard]]
  auto device() const noexcept -> IDirect3DDevice9Ptr const&;

  auto reset(D3DPRESENT_PARAMETERS&) const -> void;

  auto execute(CommandList const&) -> void;

  [[nodiscard]] auto add_texture(IDirect3DBaseTexture9Ptr) -> Texture;
  [[nodiscard]] auto get_d3d9(Texture) const -> IDirect3DBaseTexture9Ptr;

  auto set_extensions(ext::DeviceExtensions) -> void;

  [[nodiscard]]
  auto capabilities() const -> DeviceCaps const& override;
  [[nodiscard]]
  auto get_status() const noexcept -> DeviceStatus override;

  auto reset() -> void override;

  [[nodiscard]]
  auto create_pipeline(PipelineDescriptor const&) -> Pipeline override;

  auto destroy(Pipeline) noexcept -> void override;

  [[nodiscard]]
  auto create_vertex_buffer(VertexBufferDescriptor const&,
                            gsl::span<std::byte const> initialData)
    -> VertexBuffer override;

  auto destroy(VertexBuffer) noexcept -> void override;

  [[nodiscard]]
  auto map(VertexBuffer, uDeviceSize offset, uDeviceSize size)
    -> gsl::span<std::byte> override;
  auto unmap(VertexBuffer) noexcept -> void override;

  [[nodiscard]]
  auto create_index_buffer(IndexBufferDescriptor const&,
                           gsl::span<std::byte const> initialData)
    -> IndexBuffer override;

  auto destroy(IndexBuffer) noexcept -> void override;

  [[nodiscard]]
  auto map(IndexBuffer, uDeviceSize offsetInBytes, uDeviceSize sizeInBytes)
    -> gsl::span<std::byte> override;

  auto unmap(IndexBuffer) noexcept -> void override;

  [[nodiscard]]
  auto load_texture(std::filesystem::path const&) -> Texture override;

  [[nodiscard]]
  auto load_cube_texture(std::filesystem::path const&) -> Texture override;

  auto destroy(Texture) noexcept -> void override;

  [[nodiscard]]
  auto create_sampler(SamplerDescriptor const&) -> Sampler override;

  auto destroy(Sampler) noexcept -> void override;

  auto submit(gsl::span<CommandList const>) -> void override;

private:
  struct SamplerData final {
    D3DTEXTUREFILTERTYPE magFilter{D3DTEXF_POINT};
    D3DTEXTUREFILTERTYPE minFilter{D3DTEXF_POINT};
    D3DTEXTUREFILTERTYPE mipFilter{D3DTEXF_NONE};
    D3DTEXTUREADDRESS addressModeU{D3DTADDRESS_WRAP};
    D3DTEXTUREADDRESS addressModeV{D3DTADDRESS_WRAP};
    D3DTEXTUREADDRESS addressModeW{D3DTADDRESS_WRAP};
    D3DCOLOR borderColor{0};
    DWORD maxAnisotropy{1};
  };

  IDirect3DDevice9Ptr mDevice;

  ext::DeviceExtensions mExtensions;

  HandlePool<D3D9Pipeline, Pipeline> mPipelines{};
  HandlePool<IDirect3DVertexBuffer9Ptr, VertexBuffer> mVertexBuffers{};
  HandlePool<IDirect3DIndexBuffer9Ptr, IndexBuffer> mIndexBuffers{};
  HandlePool<IDirect3DBaseTexture9Ptr, Texture> mTextures{};
  HandlePool<SamplerData, Sampler> mSamplers{};

  DeviceCaps mCaps{};
  D3DPRIMITIVETYPE mCurrentPrimitiveType{D3DPT_POINTLIST};
  u32 mNumLightsUsed{};

  auto execute(Command const&) -> void;
  auto execute(CommandClearAttachments const&) -> void;
  auto execute(CommandDraw const&) -> void;
  auto execute(CommandDrawIndexed const&) -> void;
  auto execute(CommandBindPipeline const&) -> void;
  auto execute(CommandBindVertexBuffer const&) -> void;
  auto execute(CommandBindIndexBuffer const&) -> void;
  auto execute(CommandBindSampler const&) -> void;
  auto execute(CommandBindTexture const&) -> void;
  auto execute(CommandSetStencilReference const&) -> void;
  auto execute(CommandSetStencilReadMask const&) -> void;
  auto execute(CommandSetStencilWriteMask const&) -> void;
  auto execute(CommandSetBlendConstant const&) -> void;
  auto execute(CommandSetTransform const&) -> void;
  auto execute(CommandSetAmbientLight const&) -> void;
  auto execute(CommandSetLights const&) -> void;
  auto execute(CommandSetMaterial const&) -> void;
  auto execute(CommandSetFogParameters const&) -> void;
  auto execute(CommandSetReferenceAlpha const&) -> void;
  auto execute(CommandSetTextureFactor const&) -> void;
  auto execute(CommandSetTextureStageConstant const&) -> void;

  template <typename T>
  [[nodiscard]]
  auto get_extension() const -> std::shared_ptr<T>;
};

} // namespace basalt::gfx

#pragma once

#include <basalt/api/gfx/backend/device.h>

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>

#include <basalt/api/shared/handle_pool.h>

#include <wrl/client.h>

#include <filesystem>
#include <unordered_map>

namespace basalt::gfx {

struct D3D9Device final : Device {
  explicit D3D9Device(Microsoft::WRL::ComPtr<IDirect3DDevice9> device);

  [[nodiscard]] auto device() const -> Microsoft::WRL::ComPtr<IDirect3DDevice9>;

  auto reset(D3DPRESENT_PARAMETERS&) const -> void;

  auto begin_execution() const -> void;
  auto execute(const CommandList&) -> void;
  auto end_execution() const -> void;

  [[nodiscard]] auto capabilities() const -> const DeviceCaps& override;

  [[nodiscard]] auto create_pipeline(const PipelineDescriptor&)
    -> Pipeline override;

  auto destroy(Pipeline) noexcept -> void override;

  [[nodiscard]] auto
  create_vertex_buffer(const VertexBufferDescriptor&,
                       gsl::span<const std::byte> initialData)
    -> VertexBuffer override;

  void destroy(VertexBuffer) noexcept override;

  [[nodiscard]] auto map(VertexBuffer, uDeviceSize offset, uDeviceSize size)
    -> gsl::span<std::byte> override;
  void unmap(VertexBuffer) noexcept override;

  [[nodiscard]] auto load_texture(const std::filesystem::path&)
    -> Texture override;

  auto destroy(Texture) noexcept -> void override;

  [[nodiscard]] auto create_sampler(const SamplerDescriptor&)
    -> Sampler override;

  auto destroy(Sampler) noexcept -> void override;

  auto query_extension(ext::ExtensionId)
    -> std::optional<ext::ExtensionPtr> override;

private:
  using D3D9DevicePtr = Microsoft::WRL::ComPtr<IDirect3DDevice9>;
  using ExtensionMap = std::unordered_map<ext::ExtensionId, ext::ExtensionPtr>;
  using D3D9VertexBufferPtr = Microsoft::WRL::ComPtr<IDirect3DVertexBuffer9>;
  using D3D9TexturePtr = Microsoft::WRL::ComPtr<IDirect3DTexture9>;

  struct PipelineData final {
    DWORD fvf {};
    DWORD stage1Tci {0 | D3DTSS_TCI_PASSTHRU};
    D3DTEXTURETRANSFORMFLAGS stage0Ttf {D3DTTFF_DISABLE};
    DWORD stage1Arg1 {D3DTA_DIFFUSE};
    DWORD stage1Arg2 {D3DTA_TEXTURE};
    D3DTEXTUREOP stage1ColorOp {D3DTOP_DISABLE};
    D3DTEXTUREOP stage1AlphaOp {D3DTOP_DISABLE};
    D3DPRIMITIVETYPE primitiveType {D3DPT_POINTLIST};
    BOOL lighting {FALSE};
    D3DSHADEMODE shadeMode {D3DSHADE_GOURAUD};
    D3DCULL cullMode {D3DCULL_NONE};
    D3DFILLMODE fillMode {D3DFILL_SOLID};
    D3DZBUFFERTYPE zEnabled {D3DZB_FALSE};
    D3DCMPFUNC zFunc {D3DCMP_ALWAYS};
    BOOL zWriteEnabled {FALSE};
    BOOL dithering {FALSE};
  };

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

  HandlePool<PipelineData, Pipeline> mPipelines {};
  HandlePool<D3D9VertexBufferPtr, VertexBuffer> mVertexBuffers {};
  HandlePool<D3D9TexturePtr, Texture> mTextures {};
  HandlePool<SamplerData, Sampler> mSamplers {};

  DeviceCaps mCaps {};
  D3DPRIMITIVETYPE mCurrentPrimitiveType {D3DPT_POINTLIST};
  u8 mMaxLightsUsed {};

  auto execute(const Command&) -> void;
  auto execute(const CommandClearAttachments&) -> void;
  auto execute(const CommandDraw&) -> void;
  auto execute(const CommandBindPipeline&) -> void;
  auto execute(const CommandBindVertexBuffer&) -> void;
  auto execute(const CommandBindSampler&) -> void;
  auto execute(const CommandBindTexture&) -> void;
  auto execute(const CommandSetTransform&) -> void;
  void execute(const CommandSetAmbientLight&);
  void execute(const CommandSetLights&);
  void execute(const CommandSetMaterial&);
};

} // namespace basalt::gfx

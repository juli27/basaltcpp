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

  void reset(D3DPRESENT_PARAMETERS&) const;

  void begin_execution() const;
  void execute(const CommandList&);
  void end_execution() const;

  [[nodiscard]] auto capabilities() const -> const DeviceCaps& override;

  [[nodiscard]] auto create_pipeline(const PipelineDescriptor&)
    -> Pipeline override;

  void destroy(Pipeline) noexcept override;

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
  void destroy(Texture) noexcept override;

  [[nodiscard]] auto create_sampler(const SamplerDescriptor&)
    -> Sampler override;

  void destroy(Sampler) noexcept override;

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
  };

  struct SamplerData final {
    D3DTEXTUREFILTERTYPE filter {D3DTEXF_POINT};
    D3DTEXTUREFILTERTYPE mipFilter {D3DTEXF_NONE};
    D3DTEXTUREADDRESS addressModeU {D3DTADDRESS_WRAP};
    D3DTEXTUREADDRESS addressModeV {D3DTADDRESS_WRAP};
    D3DTEXTUREADDRESS addressModeW {D3DTADDRESS_WRAP};
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

  void execute(const Command&);
  void execute(const CommandClearAttachments&);
  void execute(const CommandDraw&);
  auto execute(const CommandBindPipeline&) -> void;
  void execute(const CommandBindVertexBuffer&);
  void execute(const CommandBindSampler&);
  void execute(const CommandBindTexture&);
  void execute(const CommandSetTransform&);
  void execute(const CommandSetAmbientLight&);
  void execute(const CommandSetLights&);
  void execute(const CommandSetMaterial&);
};

} // namespace basalt::gfx

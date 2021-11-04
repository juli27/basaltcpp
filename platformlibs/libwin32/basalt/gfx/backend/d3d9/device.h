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

  [[nodiscard]] auto create_pipeline(const PipelineDescriptor&)
    -> Pipeline override;

  void destroy_pipeline(Pipeline) noexcept override;

  [[nodiscard]] auto
  create_vertex_buffer(const VertexBufferDescriptor&,
                       gsl::span<const std::byte> initialData)
    -> VertexBuffer override;

  void destroy_vertex_buffer(VertexBuffer) noexcept override;

  [[nodiscard]] auto map_vertex_buffer(VertexBuffer, uDeviceSize offset,
                                       uDeviceSize size)
    -> gsl::span<std::byte> override;
  void unmap_vertex_buffer(VertexBuffer) noexcept override;

  auto load_texture(const std::filesystem::path&) -> Texture override;

  auto create_sampler(const SamplerDescriptor&) -> Sampler override;

  auto query_extension(ext::ExtensionId)
    -> std::optional<ext::ExtensionPtr> override;

private:
  using ExtensionMap = std::unordered_map<ext::ExtensionId, ext::ExtensionPtr>;
  using D3D9VertexBuffer = Microsoft::WRL::ComPtr<IDirect3DVertexBuffer9>;
  using TexturePtr = Microsoft::WRL::ComPtr<IDirect3DTexture9>;

  struct PipelineData final {
    D3DPRIMITIVETYPE primitiveType {D3DPT_POINTLIST};
    BOOL lighting {FALSE};
    D3DCULL cullMode {D3DCULL_NONE};
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

  Microsoft::WRL::ComPtr<IDirect3DDevice9> mDevice;

  ExtensionMap mExtensions;

  HandlePool<PipelineData, Pipeline> mPipelines {};
  HandlePool<D3D9VertexBuffer, VertexBuffer> mVertexBuffers {};
  HandlePool<TexturePtr, Texture> mTextures;
  HandlePool<SamplerData, Sampler> mSamplers;

  DeviceCaps mCaps {};
  D3DCAPS9 mD3D9Caps {};
  D3DPRIMITIVETYPE mCurrentPrimitiveType {D3DPT_POINTLIST};
  u8 mMaxLightsUsed {};

  void execute(const Command&);
  void execute(const CommandClearAttachments&);
  void execute(const CommandDraw&);
  void execute(const CommandSetRenderState&);
  void execute(const CommandBindPipeline&);
  void execute(const CommandBindVertexBuffer&);
  void execute(const CommandBindSampler&);
  void execute(const CommandBindTexture&);
  void execute(const CommandSetTransform&);
  void execute(const CommandSetAmbientLight&);
  void execute(const CommandSetDirectionalLights&);
  void execute(const CommandSetMaterial&);
  void execute(const CommandSetTextureStageState&);
};

} // namespace basalt::gfx

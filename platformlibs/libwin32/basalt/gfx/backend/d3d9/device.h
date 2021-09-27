#pragma once

#include <basalt/api/gfx/backend/device.h>

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>

#include <basalt/api/gfx/backend/commands.h>
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

  auto create_vertex_buffer(gsl::span<const std::byte> data,
                            const VertexLayout&) -> VertexBuffer override;

  auto load_texture(const std::filesystem::path&) -> Texture override;

  auto create_sampler(const SamplerDescriptor&) -> Sampler override;

  auto query_extension(ext::ExtensionId)
    -> std::optional<ext::ExtensionPtr> override;

private:
  using ExtensionMap = std::unordered_map<ext::ExtensionId, ext::ExtensionPtr>;
  using D3D9VertexBuffer = Microsoft::WRL::ComPtr<IDirect3DVertexBuffer9>;
  using TexturePtr = Microsoft::WRL::ComPtr<IDirect3DTexture9>;

  struct SamplerData final {
    D3DTEXTUREFILTERTYPE filter {D3DTEXF_POINT};
    D3DTEXTUREFILTERTYPE mipFilter {D3DTEXF_NONE};
    D3DTEXTUREADDRESS addressModeU {D3DTADDRESS_WRAP};
    D3DTEXTUREADDRESS addressModeV {D3DTADDRESS_WRAP};
    D3DTEXTUREADDRESS addressModeW {D3DTADDRESS_WRAP};
  };

  Microsoft::WRL::ComPtr<IDirect3DDevice9> mDevice;

  ExtensionMap mExtensions;

  HandlePool<D3D9VertexBuffer, VertexBuffer> mVertexBuffers;
  HandlePool<TexturePtr, Texture> mTextures;
  HandlePool<SamplerData, Sampler> mSamplers;

  D3DCAPS9 mDeviceCaps {};
  u8 mMaxLightsUsed {};

  // TODO: make these return bool / an error ?
  void execute(const CommandClearAttachments&) const;
  void execute(const CommandDraw&) const;
  void execute(const CommandSetDirectionalLights&);
  void execute(const CommandSetTransform&) const;
  void execute(const CommandSetMaterial&) const;
  void execute(const CommandSetRenderState&) const;
  void execute(const CommandBindTexture&) const;
  void execute(const CommandSetTextureStageState&) const;
  void execute(const CommandBindSampler&) const;
};

} // namespace basalt::gfx

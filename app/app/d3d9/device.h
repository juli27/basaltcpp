#pragma once

#include <api/gfx/backend/device.h>

#include "d3d9_custom.h"

#include <api/gfx/backend/commands.h>
#include <api/shared/handle_pool.h>

#include <wrl/client.h>

#include <unordered_map>
#include <vector>

namespace basalt::gfx {

struct D3D9Device final : Device {
  explicit D3D9Device(Microsoft::WRL::ComPtr<IDirect3DDevice9> device);

  [[nodiscard]] auto device() const -> Microsoft::WRL::ComPtr<IDirect3DDevice9>;

  void before_reset();
  void after_reset();

  void begin_execution() const;
  void execute(const CommandList&);
  void end_execution() const;

  auto create_vertex_buffer(gsl::span<const std::byte> data,
                            const VertexLayout&) -> VertexBuffer override;

  auto add_texture(std::string_view filePath) -> Texture override;

  auto query_extension(ext::ExtensionId)
    -> std::optional<ext::ExtensionPtr> override;

private:
  using ExtensionMap = std::unordered_map<ext::ExtensionId, ext::ExtensionPtr>;
  using D3D9VertexBuffer = Microsoft::WRL::ComPtr<IDirect3DVertexBuffer9>;
  using TexturePtr = Microsoft::WRL::ComPtr<IDirect3DTexture9>;

  Microsoft::WRL::ComPtr<IDirect3DDevice9> mDevice;

  ExtensionMap mExtensions;

  HandlePool<D3D9VertexBuffer, VertexBuffer> mVertexBuffers;
  HandlePool<TexturePtr, Texture> mTextures;

  D3DCAPS9 mDeviceCaps {};
  u8 mMaxLightsUsed {};

  // TODO: make these return bool / an error ?
  void execute(const CommandClear&) const;
  void execute(const CommandDraw&) const;
  void execute(const CommandSetDirectionalLights&);
  void execute(const CommandSetTransform&) const;
  void execute(const CommandSetMaterial&) const;
  void execute(const CommandSetRenderState&) const;
  void execute(const CommandSetTexture&) const;
  void execute(const CommandSetTextureStageState&) const;
};

} // namespace basalt::gfx

#pragma once

#include <basalt/gfx/backend/ext/dear_imgui_renderer.h>

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>
#include <basalt/gfx/backend/d3d9/types.h>

#include <basalt/gfx/backend/types.h>

#include <wrl/client.h>

namespace basalt::gfx::ext {

class D3D9ImGuiRenderer final : public DearImGuiRenderer {
public:
  static auto create(Microsoft::WRL::ComPtr<IDirect3DDevice9>)
    -> D3D9ImGuiRendererPtr;

  auto execute(const CommandRenderDearImGui&) const -> void;
  auto invalidate_device_objects() const -> void;
  auto create_device_objects() const -> void;

  auto init() -> void override;
  auto shutdown() -> void override;
  auto new_frame() -> void override;

  explicit D3D9ImGuiRenderer(Microsoft::WRL::ComPtr<IDirect3DDevice9>);

private:
  Microsoft::WRL::ComPtr<IDirect3DDevice9> mDevice;
};

} // namespace basalt::gfx::ext

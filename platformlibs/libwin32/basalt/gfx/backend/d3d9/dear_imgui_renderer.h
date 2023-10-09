#pragma once

#include <basalt/gfx/backend/ext/dear_imgui_renderer.h>

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>
#include <basalt/gfx/backend/d3d9/types.h>

#include <basalt/gfx/backend/types.h>

namespace basalt::gfx::ext {

class D3D9ImGuiRenderer final : public DearImGuiRenderer {
public:
  static auto create(IDirect3DDevice9Ptr) -> D3D9ImGuiRendererPtr;

  auto execute(CommandRenderDearImGui const&) const -> void;
  auto invalidate_device_objects() const -> void;
  auto create_device_objects() const -> void;

  auto init() -> void override;
  auto shutdown() -> void override;
  auto new_frame() -> void override;

  explicit D3D9ImGuiRenderer(IDirect3DDevice9Ptr);

private:
  IDirect3DDevice9Ptr mDevice;
};

} // namespace basalt::gfx::ext

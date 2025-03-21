#include "dear_imgui_renderer.h"

#include "d3d9_error.h"

#include <utility>

#include <imgui.h>
#include <imgui_impl_dx9.h>

namespace basalt::gfx::ext {

auto D3D9ImGuiRenderer::create(IDirect3DDevice9Ptr device)
  -> D3D9ImGuiRendererPtr {
  return std::make_shared<D3D9ImGuiRenderer>(std::move(device));
}

auto D3D9ImGuiRenderer::execute(CommandRenderDearImGui const&) const -> void {
  ImGui::Render();
  if (auto* drawData = ImGui::GetDrawData()) {
    // the imgui d3d9 renderer doesn't set its coordinate source
    // and texture coords transform flags
    D3D9CHECK(mDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,
                                            0 | D3DTSS_TCI_PASSTHRU));
    D3D9CHECK(mDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
                                            D3DTTFF_DISABLE));

    ImGui_ImplDX9_RenderDrawData(drawData);
  }
}

auto D3D9ImGuiRenderer::invalidate_device_objects() const -> void {
  ImGui_ImplDX9_InvalidateDeviceObjects();
}

auto D3D9ImGuiRenderer::create_device_objects() const -> void {
  ImGui_ImplDX9_CreateDeviceObjects();
}

auto D3D9ImGuiRenderer::init() -> void {
  ImGui_ImplDX9_Init(mDevice.Get());
  ImGui_ImplDX9_CreateDeviceObjects();
}

auto D3D9ImGuiRenderer::shutdown() -> void {
  ImGui_ImplDX9_Shutdown();
}

auto D3D9ImGuiRenderer::new_frame() -> void {
  ImGui_ImplDX9_NewFrame();
}

D3D9ImGuiRenderer::D3D9ImGuiRenderer(IDirect3DDevice9Ptr device)
  : mDevice{std::move(device)} {
}

} // namespace basalt::gfx::ext

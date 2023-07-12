#include <basalt/gfx/backend/d3d9/dear_imgui_renderer.h>

#include <utility>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>

namespace basalt::gfx::ext {

using Microsoft::WRL::ComPtr;

auto D3D9ImGuiRenderer::create(ComPtr<IDirect3DDevice9> device)
  -> D3D9ImGuiRendererPtr {
  return std::make_shared<D3D9ImGuiRenderer>(std::move(device));
}

auto D3D9ImGuiRenderer::execute(const CommandRenderDearImGui&) const -> void {
  ImGui::Render();
  if (auto* drawData {ImGui::GetDrawData()}) {
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

D3D9ImGuiRenderer::D3D9ImGuiRenderer(ComPtr<IDirect3DDevice9> device)
  : mDevice {std::move(device)} {
}

} // namespace basalt::gfx::ext

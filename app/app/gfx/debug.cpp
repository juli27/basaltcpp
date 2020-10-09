#include "debug.h"

#include "utils.h"

#include <imgui/imgui.h>

namespace basalt::gfx {

void Debug::update(const AdapterInfo& currentAdapter) {
  // https://github.com/ocornut/imgui/issues/331
  enum class OpenPopup : u8 { None, GfxInfo };
  OpenPopup shouldOpenPopup {OpenPopup::None};

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      ImGui::Separator();

      if (ImGui::MenuItem("GFX Info...")) {
        shouldOpenPopup = OpenPopup::GfxInfo;
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  if (shouldOpenPopup == OpenPopup::GfxInfo) {
    ImGui::OpenPopup("Gfx Info");
  }

  if (ImGui::BeginPopupModal("Gfx Info", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("GFX Adapter: %s", currentAdapter.displayName.c_str());
    ImGui::Text("Driver: %s (%s)", currentAdapter.driver.c_str(),
                currentAdapter.driverVersion.c_str());

    ImGui::Separator();

    ImGui::TextUnformatted("Adapter Modes");

    if (ImGui::BeginChild("modes", ImVec2 {0, 250})) {
      for (const auto& adapterMode : currentAdapter.adapterModes) {
        ImGui::Selectable(to_string(adapterMode).c_str(), false,
                          ImGuiSelectableFlags_DontClosePopups);
      }
    }

    ImGui::EndChild();

    if (ImGui::Button("Close", ImVec2 {120.0f, 0.0f})) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }
}

} // namespace basalt::gfx

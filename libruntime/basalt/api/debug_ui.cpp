#include <basalt/api/debug_ui.h>

#include <basalt/gfx/utils.h>

#include <fmt/format.h>
#include <imgui/imgui.h>

namespace basalt {

auto DebugUi::show_gfx_info(const gfx::Info& gfxInfo) -> void {
  if (ImGui::BeginPopupModal("Gfx Info", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    static u32 currentIndex {0};
    const gfx::AdapterInfo& current {gfxInfo.adapters[currentIndex]};
    if (ImGui::BeginCombo("GFX Adapter", current.displayName.c_str())) {
      for (const auto& adapter : gfxInfo.adapters) {
        const bool isSelected {adapter.adapterIndex == currentIndex};
        if (ImGui::Selectable(
              fmt::format("{} ##{}", adapter.displayName, adapter.adapterIndex)
                .c_str(),
              isSelected)) {
          currentIndex = adapter.adapterIndex;
        }

        if (isSelected) {
          ImGui::SetItemDefaultFocus();
        }
      }

      ImGui::EndCombo();
    }

    ImGui::Text("Driver: %s", current.driverInfo.c_str());

    ImGui::Separator();

    ImGui::TextUnformatted("Adapter Modes");

    if (ImGui::BeginChild("modes", ImVec2 {0, 250})) {
      for (const auto& mode : current.adapterModes) {
        ImGui::Selectable(to_string(mode).c_str(), false,
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

} // namespace basalt

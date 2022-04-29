#include <basalt/api/debug_ui.h>

#include <basalt/gfx/utils.h>

#include <basalt/api/base/enum_array.h>

#include <imgui/imgui.h>

#include <string_view>

namespace basalt {

using namespace std::literals;
using std::string_view;

namespace {

auto to_string(const gfx::BackendApi api) -> string_view {
  static constexpr EnumArray<gfx::BackendApi, string_view, 2> TO_STRING {
    {gfx::BackendApi::Default, "Default"sv},
    {gfx::BackendApi::Direct3D9, "Direct3D 9"sv},
  };
  static_assert(gfx::BACKEND_API_COUNT == TO_STRING.size());

  return TO_STRING[api];
}

} // namespace

auto DebugUi::show_gfx_info(const gfx::Info& gfxInfo) -> void {
  const gfx::AdapterInfo& current {gfxInfo.adapters[mSelectedAdapter]};

  ImGui::Text("Backend API: %s", to_string(gfxInfo.backendApi).data());

  ImGui::Separator();

  if (ImGui::BeginCombo("Adapter", current.displayName.c_str())) {
    for (const auto& adapter : gfxInfo.adapters) {
      ImGui::PushID(&adapter);

      const bool isSelected {adapter.adapterIndex == mSelectedAdapter};

      if (ImGui::Selectable(adapter.displayName.c_str(), isSelected)) {
        mSelectedAdapter = adapter.adapterIndex;
      }

      if (isSelected) {
        ImGui::SetItemDefaultFocus();
      }

      ImGui::PopID();
    }

    ImGui::EndCombo();
  }

  ImGui::Text("Driver: %s", current.driverInfo.c_str());

  ImGui::Separator();

  ImGui::TextUnformatted("Adapter Modes");

  if (ImGui::BeginChild("modes")) {
    for (const auto& mode : current.adapterModes) {
      ImGui::TextUnformatted(to_string(mode).c_str());
    }
  }

  ImGui::EndChild();
}

} // namespace basalt

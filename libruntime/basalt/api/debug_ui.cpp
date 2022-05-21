#include <basalt/api/debug_ui.h>

#include <basalt/gfx/utils.h>

#include <basalt/api/base/enum_array.h>

#include <fmt/format.h>
#include <imgui/imgui.h>

#include <numeric>
#include <string>
#include <string_view>

namespace basalt {

using namespace std::literals;
using std::string;
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

auto to_string(const gfx::DisplayMode& mode,
               const gfx::ImageFormat format) noexcept -> string {
  const auto gcd {std::gcd(mode.width, mode.height)};

  return fmt::format(FMT_STRING("{}x{} ({}:{}) {}Hz {}"), mode.width,
                     mode.height, mode.width / gcd, mode.height / gcd,
                     mode.refreshRate, to_string(format));
}

} // namespace

auto DebugUi::show_gfx_info(const gfx::Info& gfxInfo) -> void {
  const gfx::AdapterInfo& selectedAdapter {
    gfxInfo.adapters[mSelectedAdapterIndex]};

  ImGui::Text("Backend API: %s", to_string(gfxInfo.backendApi).data());

  ImGui::Separator();

  if (ImGui::BeginCombo("Adapter", selectedAdapter.displayName.c_str())) {
    for (const auto& adapter : gfxInfo.adapters) {
      ImGui::PushID(&adapter);

      const u32 adapterIndex {adapter.handle.value()};
      const bool isSelected {adapterIndex == mSelectedAdapterIndex};

      if (ImGui::Selectable(adapter.displayName.c_str(), isSelected)) {
        mSelectedAdapterIndex = adapterIndex;
      }

      if (isSelected) {
        ImGui::SetItemDefaultFocus();
      }

      ImGui::PopID();
    }

    ImGui::EndCombo();
  }

  ImGui::Text("Driver: %s", selectedAdapter.driverInfo.c_str());

  ImGui::Separator();

  ImGui::TextUnformatted("Adapter Modes");

  if (ImGui::BeginChild("modes")) {
    const auto& adapterModes {selectedAdapter.adapterModes};

    for (uSize i {0}; i < adapterModes.size(); ++i) {
      const auto& adapterMode {adapterModes[i]};

      for (const auto& displayMode : adapterMode.displayModes) {
        if (adapterMode.displayFormat == selectedAdapter.displayFormat &&
            displayMode.width == selectedAdapter.displayMode.width &&
            displayMode.height == selectedAdapter.displayMode.height &&
            displayMode.refreshRate ==
              selectedAdapter.displayMode.refreshRate) {
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4 {1.0f, 1.0f, 0.0f, 1.0f});
          ImGui::TextUnformatted(
            to_string(displayMode, adapterMode.displayFormat).c_str());
          ImGui::PopStyleColor();

          if (ImGui::IsWindowAppearing()) {
            ImGui::SetScrollHereY();
          }
        } else {
          ImGui::TextUnformatted(
            to_string(displayMode, adapterMode.displayFormat).c_str());
        }
      }
    }
  }

  ImGui::EndChild();
}

} // namespace basalt

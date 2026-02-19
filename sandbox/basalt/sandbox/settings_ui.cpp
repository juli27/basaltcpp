#include "settings_ui.h"

#include "settings.h"

#include <basalt/api/base/utils.h>

#include <fmt/format.h>
#include <imgui.h>

using namespace basalt;

namespace {

auto to_string(gfx::MultiSampleCount count) -> std::string {
  return std::to_string(enum_cast(count) + 1);
}

} // namespace

auto SettingsUi::show_settings_editor(Settings& settings,
                                      gfx::Info const& gfxInfo, bool* open)
  -> void {
  if (!ImGui::Begin("Settings##Sandbox", open)) {
    ImGui::End();

    return;
  }

  if (ImGui::IsWindowAppearing()) {
    // TODO: copy settings to a temporary object to allow for canceling changes
  }

  settings_editor(settings, gfxInfo);

  if (ImGui::Button("Save")) {
    settings.to_file();
  }
  ImGui::SameLine();
  ImGui::TextUnformatted("Restart to apply changes");

  ImGui::End();
}

auto SettingsUi::settings_editor(Settings& settings, gfx::Info const& gfxInfo)
  -> void {
  if (ImGui::BeginTable("SettingsTable", 2, ImGuiTableFlags_BordersInnerV)) {
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableNextColumn();
    ImGui::TextUnformatted("Initial window mode");
    ImGui::TableNextColumn();
    ImGui::PushItemWidth(-FLT_MIN);
    window_mode_combo("##WindowMode", settings.windowMode);

    ImGui::TableNextColumn();
    ImGui::TextUnformatted("Adapter");
    ImGui::TableNextColumn();
    adapter_combo("##adapter", settings.adapter, gfxInfo);

    auto const& adapterInfo = gfxInfo.adapterInfos[settings.adapter];

    ImGui::TableNextColumn();
    ImGui::TextUnformatted("Multi-sample count");

    auto const availableSampleCounts = [&] {
      auto const& sharedModeInfo = adapterInfo.sharedModeInfo;
      auto const& backBufferFormats = sharedModeInfo.backBufferFormats;
      for (auto const& format : backBufferFormats) {
        // TODO: remove hardcoded formats
        auto const isCurrent =
          format.renderTargetFormat == gfx::ImageFormat::B8G8R8X8 &&
          format.depthStencilFormat == gfx::ImageFormat::D24S8;

        if (isCurrent) {
          return format.supportedSampleCounts;
        }
      }

      return gfx::MultiSampleCounts{gfx::MultiSampleCount::One};
    }();
    ImGui::TableNextColumn();
    multi_sample_count_combo("##MultiSampleCount", settings.multiSampleCount,
                             availableSampleCounts);

    ImGui::TableNextColumn();
    ImGui::TextUnformatted("Exclusive fullscreen mode");

    auto const& displayModes = [&] {
      for (auto const& mode : adapterInfo.exclusiveModes) {
        if (mode.displayFormat == gfx::ImageFormat::B8G8R8X8) {
          return mode.displayModes;
        }
      }

      return gfx::DisplayModes{};
    }();
    ImGui::TableNextColumn();
    display_mode_combo("##DisplayMode", settings.displayMode, displayModes);

    ImGui::EndTable();
  }
}

auto SettingsUi::window_mode_combo(char const* label, WindowMode& windowMode)
  -> void {
  auto current = i32{enum_cast(windowMode)};
  ImGui::Combo(label, &current,
               "Windowed\0Fullscreen\0Fullscreen (exclusive)\0");
  windowMode = WindowMode{static_cast<u8>(current)};
}

auto SettingsUi::adapter_combo(char const* label, u32& adapter,
                               gfx::Info const& gfxInfo) -> void {
  auto const& adapters = gfxInfo.adapterInfos;
  if (adapter >= adapters.size()) {
    adapter = gfxInfo.currentAdapter;
  }

  auto const toString = [&](gfx::AdapterInfo const& info) {
    return fmt::format(FMT_STRING("{}: {}"), info.index,
                       info.identifier.displayName);
  };

  auto const& currentAdapterInfo = adapters[adapter];
  if (ImGui::BeginCombo(label, toString(currentAdapterInfo).c_str())) {
    for (auto const& info : adapters) {
      if (ImGui::Selectable(toString(info).c_str(), info.index == adapter)) {
        adapter = info.index;
      }
    }

    ImGui::EndCombo();
  }
}

auto SettingsUi::multi_sample_count_combo(
  char const* label, gfx::MultiSampleCount& current,
  gfx::MultiSampleCounts const& availableCounts) -> void {
  if (ImGui::BeginCombo(label, to_string(current).c_str())) {
    for (auto const count :
         {gfx::MultiSampleCount::One, gfx::MultiSampleCount::Two,
          gfx::MultiSampleCount::Four, gfx::MultiSampleCount::Eight,
          gfx::MultiSampleCount::Sixteen}) {
      if (!availableCounts[count]) {
        continue;
      }

      auto const isCurrent = current == count;
      if (ImGui::Selectable(to_string(count).c_str(), isCurrent)) {
        current = count;
      }

      if (isCurrent) {
        ImGui::SetItemDefaultFocus();
      }
    }

    ImGui::EndCombo();
  }
}

namespace {

// TODO: move to runtime
auto operator==(gfx::DisplayMode const& l, gfx::DisplayMode const& r) -> bool {
  return l.width == r.width && l.height == r.height &&
         l.refreshRate == r.refreshRate;
}

} // namespace

auto SettingsUi::display_mode_combo(char const* label,
                                    gfx::DisplayMode& current,
                                    gfx::DisplayModes const& modes) -> void {
  auto const previewString =
    fmt::format(FMT_STRING("{} x {}, {} Hz"), current.width, current.height,
                current.refreshRate);

  if (ImGui::BeginCombo(label, previewString.c_str())) {
    for (auto const& mode : modes) {
      auto const modeString =
        fmt::format(FMT_STRING("{} x {}, {} Hz"), mode.width, mode.height,
                    mode.refreshRate);

      auto const isCurrent = mode == current;
      if (ImGui::Selectable(modeString.c_str(), isCurrent)) {
        current = mode;
      }
    }

    ImGui::EndCombo();
  }
}

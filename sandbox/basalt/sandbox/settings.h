#pragma once

#include <basalt/api/canvas.h>

#include <basalt/api/gfx/backend/adapter.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/base/types.h>

#include <filesystem>
#include <optional>

auto get_settings_file_path() -> std::filesystem::path;

struct Settings {
  basalt::CanvasMode canvasMode{basalt::CanvasMode::Windowed};
  basalt::u32 adapter{0};
  basalt::gfx::MultiSampleCount multiSampleCount{
    basalt::gfx::MultiSampleCount::One};
  basalt::gfx::DisplayMode displayMode{};
  // - last fullscreen mode (exclusive, non-exclusive)
  // - last window mode (restored, maximized, fullscreen)
  // - window size

  static auto from_file(std::filesystem::path const&)
    -> std::optional<Settings>;

  auto to_file(std::filesystem::path const& filePath) const -> void;
};

constexpr auto operator==(Settings const& lhs, Settings const& rhs) -> bool {
  return lhs.canvasMode == rhs.canvasMode && lhs.adapter == rhs.adapter &&
         lhs.multiSampleCount == rhs.multiSampleCount &&
         lhs.displayMode == rhs.displayMode;
}

constexpr auto operator!=(Settings const& lhs, Settings const& rhs) -> bool {
  return !(lhs == rhs);
}

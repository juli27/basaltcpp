#pragma once

#include <basalt/api/gfx/backend/adapter.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/types.h>

#include <basalt/api/base/types.h>

#include <filesystem>
#include <optional>

struct Settings {
  std::filesystem::path filePath;

  basalt::WindowMode windowMode{basalt::WindowMode::Windowed};
  basalt::u32 adapter{};
  basalt::gfx::MultiSampleCount multiSampleCount{
    basalt::gfx::MultiSampleCount::One};
  basalt::gfx::DisplayMode displayMode;
  // - last fullscreen mode (exclusive, non-exclusive)
  // - last window mode (restored, maximized, fullscreen)
  // - window size

  static auto
  from_file(std::filesystem::path const&) -> std::optional<Settings>;

  auto to_file() const -> void;
};

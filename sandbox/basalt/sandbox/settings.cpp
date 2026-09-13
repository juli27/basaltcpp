#include "settings.h"

#include <basalt/api/base/log.h>
#include <basalt/api/base/utils.h>

#include <fmt/ostream.h>

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>

using namespace basalt;
using namespace std::literals;

namespace {

auto parse_canvas_mode(toml::node_view<toml::node const> const node)
  -> std::optional<CanvasMode> {
  auto const value = node.value<std::string>();
  if (!value) {
    return std::nullopt;
  }

  if (*value == "Windowed"sv) {
    return CanvasMode::Windowed;
  }
  if (*value == "Fullscreen"sv) {
    return CanvasMode::Fullscreen;
  }
  if (*value == "FullscreenExclusive"sv) {
    return CanvasMode::FullscreenExclusive;
  }

  return std::nullopt;
}

auto to_toml(CanvasMode const canvasMode) -> std::string_view {
  switch (canvasMode) {
  case CanvasMode::Windowed:
    return "Windowed"sv;
  case CanvasMode::Fullscreen:
    return "Fullscreen"sv;
  case CanvasMode::FullscreenExclusive:
    return "FullscreenExclusive"sv;
  }

  BASALT_CRASH("unhandled CanvasMode value");
}

auto parse_multi_sample_count(toml::node_view<toml::node const> const node)
  -> std::optional<gfx::MultiSampleCount> {
  auto const value = node.value<u8>();
  if (!value) {
    return std::nullopt;
  }

  auto const enumValue = static_cast<u8>(*value - 1);
  if (enumValue >= gfx::MULTI_SAMPLE_COUNT_COUNT) {
    return std::nullopt;
  }

  return gfx::MultiSampleCount{enumValue};
}

auto to_toml(gfx::MultiSampleCount const multiSampleCount) -> u8 {
  return enum_cast(multiSampleCount) + 1;
}

auto parse_display_mode(toml::node_view<toml::node const> const node)
  -> std::optional<gfx::DisplayMode> {
  auto const width = node["width"sv].value<u32>();
  auto const height = node["height"sv].value<u32>();
  auto const refreshRate = node["refresh_rate"sv].value<u32>();
  if (!width || !height || !refreshRate) {
    return std::nullopt;
  }

  return gfx::DisplayMode{*width, *height, *refreshRate};
}

auto to_toml(gfx::DisplayMode const& displayMode) -> toml::table {
  auto displayModeTable = toml::table{
    {"width"sv, displayMode.width},
    {"height"sv, displayMode.height},
    {"refresh_rate"sv, displayMode.refreshRate},
  };
  displayModeTable.is_inline(true);

  return displayModeTable;
}

auto parse_settings(toml::table const& table) -> Settings {
  // start with default settings
  // Parsing is lenient. Missing keys keep their default value
  auto settings = Settings{};
  if (auto const canvasMode = parse_canvas_mode(table["mode"sv])) {
    settings.canvasMode = *canvasMode;
  }
  if (auto const adapter = table["adapter"sv].value<u32>()) {
    settings.adapter = *adapter;
  }
  if (auto const multiSampleCount =
        parse_multi_sample_count(table["multi_sample_count"sv])) {
    settings.multiSampleCount = *multiSampleCount;
  }
  if (auto const displayMode = parse_display_mode(table["display_mode"sv])) {
    settings.displayMode = *displayMode;
  }

  return settings;
}

auto to_toml(Settings const& settings) -> toml::table {
  return toml::table{
    {"mode"sv, to_toml(settings.canvasMode)},
    {"adapter"sv, settings.adapter},
    {"multi_sample_count"sv, to_toml(settings.multiSampleCount)},
    {"display_mode"sv, to_toml(settings.displayMode)},
  };
}

} // namespace

auto get_settings_file_path() -> std::filesystem::path {
  return std::filesystem::u8path("settings.toml"sv);
}

auto Settings::from_file(std::filesystem::path const& filePath)
  -> std::optional<Settings> {
  auto file = std::ifstream{filePath, std::ifstream::binary};
  if (!file) {
    BASALT_LOG_ERROR("Failed to open settings file");
    return std::nullopt;
  }

  auto const parseResult = toml::parse(file, filePath.u8string());
  if (!parseResult) {
    auto const& error = parseResult.error();
    BASALT_LOG_ERROR("Failed to parse settings file: {}", error.description());
    BASALT_LOG_ERROR("\t{}", fmt::streamed(error.source()));

    return std::nullopt;
  }
  file.close();

  return parse_settings(parseResult.table());
}

auto Settings::to_file(std::filesystem::path const& filePath) const -> void {
  auto file = std::ofstream{filePath, std::ofstream::binary};
  if (!file) {
    BASALT_LOG_ERROR("Failed to open settings file for writing");
    return;
  }

  file << to_toml(*this) << '\n';
}

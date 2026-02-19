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

using namespace basalt;
using namespace std::literals;

namespace {

constexpr auto to_sample_count(u8 const num)
  -> std::optional<gfx::MultiSampleCount> {
  if (num >= gfx::MULTI_SAMPLE_COUNT_COUNT) {
    return std::nullopt;
  }

  return gfx::MultiSampleCount{num};
}

} // namespace

auto Settings::from_file(std::filesystem::path const& filePath)
  -> std::optional<Settings> {
  auto file =
    std::ifstream{filePath, std::ifstream::in | std::ifstream::binary};
  if (!file.is_open()) {
    BASALT_LOG_INFO("Failed to open settings file");
    return std::nullopt;
  }

  auto parseResult = toml::parse(file, filePath.u8string());
  if (!parseResult) {
    auto const& error = parseResult.error();
    BASALT_LOG_INFO("Failed to parse settings file: {}", error.description());
    BASALT_LOG_INFO("\t{}", fmt::streamed(error.source()));

    return std::nullopt;
  }
  file.close();

  auto const& table = parseResult.table();

  auto settings = Settings{filePath};
  settings.windowMode = to_window_mode(table["mode"].value_or(i32{0}));
  settings.adapter = table["adapter"].value_or(u32{0});
  settings.multiSampleCount =
    to_sample_count(table["multiSampleCount"].value_or(u8{0}))
      .value_or(gfx::MultiSampleCount::One);

  auto const displayModeTable = table["displayMode"];
  auto& displayMode = settings.displayMode;
  displayMode.width = displayModeTable["width"].value_or(u32{0});
  displayMode.height = displayModeTable["height"].value_or(u32{0});
  displayMode.refreshRate = displayModeTable["refreshRate"].value_or(u32{0});

  return settings;
}

auto Settings::to_file() const -> void {
  auto const table = toml::table{
    {"mode"s, enum_cast(windowMode)},
    {"adapter"s, adapter},
    {"multiSampleCount"s, enum_cast(multiSampleCount)},
    {"displayMode"s,
     toml::table{
       {"width"s, displayMode.width},
       {"height"s, displayMode.height},
       {"refreshRate"s, displayMode.refreshRate},
     }},
  };

  auto file =
    std::ofstream{filePath, std::ofstream::out | std::ofstream::binary |
                              std::ofstream::trunc};
  if (!file.is_open()) {
    BASALT_LOG_ERROR("Failed to open settings file for writing");
    return;
  }

  file << table << '\n';
}

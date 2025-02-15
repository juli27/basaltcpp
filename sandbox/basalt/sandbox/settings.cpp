#include "settings.h"

#include <basalt/api/base/log.h>
#include <basalt/api/base/utils.h>

#include <toml++/toml.hpp>

#include <filesystem>
#include <optional>
#include <string>

using namespace basalt;
using namespace std::literals;

using std::nullopt;
using std::ofstream;
using std::optional;
using std::filesystem::path;

namespace {

constexpr auto
to_sample_count(i32 const num) noexcept -> gfx::MultiSampleCount {
  if (num >= gfx::MULTI_SAMPLE_COUNT_COUNT || num < 0) {
    return gfx::MultiSampleCount::One;
  }

  return gfx::MultiSampleCount{static_cast<u8>(num)};
}

} // namespace

auto Settings::from_file(path const& filePath) -> optional<Settings> {
  toml::table table;
  try {
    table = toml::parse_file(filePath.native());
  } catch (toml::parse_error const& e) {
    BASALT_LOG_WARN("Failed to parse settings file: {}", e.what());

    return nullopt;
  }

  auto settings = Settings{};
  settings.filePath = filePath;
  settings.windowMode = to_window_mode(table["mode"].value_or(0));
  settings.adapter = table["adapter"].value_or(0);
  settings.multiSampleCount =
    to_sample_count(table["multiSampleCount"].value_or(0));

  if (auto displayModeTable = table["DisplayMode"].as_table()) {
    auto displayMode = gfx::DisplayMode{};
    displayMode.width = (*displayModeTable)["width"].value_or(0);
    displayMode.height = (*displayModeTable)["height"].value_or(0);
    displayMode.refreshRate = (*displayModeTable)["refreshRate"].value_or(0);

    settings.displayMode = displayMode;
  }

  return settings;
}

auto Settings::to_file() const -> void {
  auto table = toml::table{};
  table.insert_or_assign("mode"s, enum_cast(windowMode));
  table.insert_or_assign("adapter"s, adapter);
  table.insert_or_assign("multiSampleCount"s, enum_cast(multiSampleCount));

  auto displayModeTable = toml::table{};
  displayModeTable.insert_or_assign("width"s, displayMode.width);
  displayModeTable.insert_or_assign("height"s, displayMode.height);
  displayModeTable.insert_or_assign("refreshRate"s, displayMode.refreshRate);

  table.insert_or_assign("DisplayMode"s, displayModeTable);

  auto const flags = std::ios::out | std::ios::binary | std::ios::trunc;
  auto file = ofstream{filePath, flags};
  file << table << std::endl;
}

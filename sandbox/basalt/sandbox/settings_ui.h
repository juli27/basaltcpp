#pragma once

#include "settings.h"

#include <basalt/api/gfx/info.h>
#include <basalt/api/gfx/backend/adapter.h>

#include <basalt/api/shared/types.h>

#include <basalt/api/base/types.h>

class SettingsUi {
public:
  // returns true when settings should be saved
  static auto show_settings_editor(Settings&, basalt::gfx::Info const&,
                                   bool* open) -> void;

  static auto settings_editor(Settings&, basalt::gfx::Info const&) -> void;

  static auto window_mode_combo(char const* label, basalt::WindowMode&) -> void;

  static auto adapter_combo(char const* label, basalt::u32& adapter,
                            basalt::gfx::Info const&) -> void;

  static auto multi_sample_count_combo(char const* label,
                                       basalt::gfx::MultiSampleCount&,
                                       basalt::gfx::MultiSampleCounts const&)
    -> void;

  static auto display_mode_combo(char const* label, basalt::gfx::DisplayMode&,
                                 basalt::gfx::DisplayModes const&) -> void;
};

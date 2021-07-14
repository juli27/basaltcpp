#pragma once

#include <basalt/api/shared/config.h>
#include <basalt/api/shared/types.h>

#include <basalt/api/base/utils.h>

#define BASALT_CONFIG_DEFAULTS                                                 \
  {"runtime.debugUI.enabled"s, true}, {"window.title"s, "Basalt Sandbox"s},    \
    {"window.resizeable"s, true},                                              \
    {"window.mode"s, enum_cast(basalt::WindowMode::Windowed)},                 \
    {"window.surface.windowedSize.width"s, 0},                                 \
    {"window.surface.windowedSize.height"s, 0},                                \
    {"gfx.backend.api"s, enum_cast(basalt::GfxBackendApi::Default)},

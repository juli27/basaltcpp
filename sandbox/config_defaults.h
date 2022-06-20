#pragma once

#include <basalt/api/gfx/types.h>

#include <basalt/api/shared/types.h>

#include <basalt/api/base/utils.h>

#define BASALT_CONFIG_DEFAULTS                                                 \
  {"debug.scene_inspector.enabled"s, false},                                   \
    {"runtime.debugUI.enabled"s, true}, {"window.title"s, "Basalt Sandbox"s},  \
    {"window.resizeable"s, true},                                              \
    {"window.mode"s, ::basalt::enum_cast(::basalt::WindowMode::Windowed)},     \
    {"window.size.width"s, 0}, {"window.size.height"s, 0},                     \
    {"gfx.backend.api"s,                                                       \
     ::basalt::enum_cast(::basalt::gfx::BackendApi::Default)},

#pragma once
#ifndef BASALT_APP_GLOBALS_H
#define BASALT_APP_GLOBALS_H

#include "runtime/platform/Platform.h"

#include "runtime/shared/Config.h"
#include "runtime/shared/Size2D.h"

#include <vector>

namespace basalt {

namespace gfx {

struct View;

} // namespace gfx

namespace win32 {

struct WindowData final {
  WindowData() noexcept = default;
  WindowData(const WindowData&) noexcept = delete;
  WindowData(WindowData&&) noexcept = default;
  ~WindowData() noexcept = default;

  auto operator=(const WindowData&) noexcept -> WindowData& = delete;
  auto operator=(WindowData&&) noexcept -> WindowData& = default;

  Size2Du16 clientAreaSize {Size2Du16::dont_care()};
  WindowMode mode {WindowMode::Windowed};
};

extern WindowData sWindowData;

extern std::vector<platform::PlatformEventCallback> sEventListener;

extern gfx::View sCurrentView;

} // namespace win32
} // namespace basalt

#endif // !BASALT_APP_GLOBALS_H

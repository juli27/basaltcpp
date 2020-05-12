#pragma once
#ifndef BASALT_RUNTIME_PLATFORM_WIN32_GLOBALS_H
#define BASALT_RUNTIME_PLATFORM_WIN32_GLOBALS_H

#include "runtime/platform/Platform.h"

#include "runtime/shared/Config.h"
#include "runtime/shared/Size2D.h"

#include <memory>
#include <vector>

namespace basalt {

struct Scene;

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

extern std::shared_ptr<Scene> sCurrentScene;

} // namespace win32
} // namespace basalt

#endif // !BASALT_RUNTIME_PLATFORM_WIN32_GLOBALS_H

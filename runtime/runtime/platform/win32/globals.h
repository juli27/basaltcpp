#pragma once
#ifndef BASALT_RUNTIME_PLATFORM_WIN32_GLOBALS_H
#define BASALT_RUNTIME_PLATFORM_WIN32_GLOBALS_H

#include "runtime/shared/win32/Windows_custom.h"

#include "runtime/platform/Platform.h"

#include "runtime/shared/Config.h"
#include "runtime/shared/Size2D.h"

#include <memory>
#include <vector>

namespace basalt::gfx::backend {
struct D3D9ContextFactory;
struct IGfxContext;
}

namespace basalt::platform {
struct Event;
}

namespace basalt::win32 {

struct WindowData final {
  WindowData() noexcept = default;
  WindowData(const WindowData&) noexcept = delete;
  WindowData(WindowData&&) noexcept = default;
  ~WindowData() noexcept = default;

  auto operator=(const WindowData&) noexcept -> WindowData& = delete;
  auto operator=(WindowData&&) noexcept -> WindowData& = default;

  HWND handle {nullptr};
  std::unique_ptr<gfx::backend::D3D9ContextFactory> factory {};
  std::unique_ptr<gfx::backend::IGfxContext> gfxContext {};
  Size2Du16 clientAreaSize {Size2Du16::dont_care()};
  WindowMode mode {WindowMode::Windowed};
  bool isResizeable {false};
  bool isMinimized {false};
  bool isSizing {false};
};

extern WindowData sWindowData;

extern std::vector<platform::PlatformEventCallback> sEventListener;
extern std::vector<std::shared_ptr<platform::Event>> sPendingEvents;

} // namespace basalt::win32

#endif // !BASALT_RUNTIME_PLATFORM_WIN32_GLOBALS_H

#pragma once

#include "types.h"

#include <basalt/win32/shared/Windows_custom.h>

#include <basalt/api/types.h>

#include <basalt/runtime.h>

#include <basalt/api/base/enum_array.h>

namespace basalt {

using MouseCursors = EnumArray<MouseCursor, HCURSOR, MOUSE_CURSOR_COUNT>;

class Win32App final {
public:
  [[nodiscard]]
  static auto init(HMODULE, int showCommand) -> Win32App;

  Win32App(Win32App const&) = delete;
  Win32App(Win32App&&) noexcept = default;

  ~Win32App() noexcept;

  auto operator=(Win32App const&) -> Win32App& = delete;
  auto operator=(Win32App&&) -> Win32App& = delete;

  auto run() -> void;

private:
  MouseCursors mMouseCursors;
  Win32AppWindowPtr mAppWindow;
  Runtime mRuntime;

  Win32App(Win32AppWindowPtr, Runtime);
};

} // namespace basalt

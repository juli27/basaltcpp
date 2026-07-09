#pragma once

#include "types.h"

#include "shared/Windows_custom.h"

#include <basalt/runtime.h>

namespace basalt {

class Win32App {
public:
  [[nodiscard]]
  static auto init(HMODULE) -> Win32App;

  Win32App(Win32App const&) = delete;
  Win32App(Win32App&&) noexcept = default;

  ~Win32App() noexcept;

  auto operator=(Win32App const&) -> Win32App& = delete;
  auto operator=(Win32App&&) -> Win32App& = delete;

  auto run(int showCommand) -> void;

private:
  Win32AppWindowPtr mAppWindow;
  Runtime mRuntime;

  Win32App(Win32AppWindowPtr, Runtime);
};

} // namespace basalt

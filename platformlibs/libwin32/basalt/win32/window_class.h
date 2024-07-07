#pragma once

#include "types.h"

#include <basalt/win32/shared/Windows_custom.h>

namespace basalt {

class Win32WindowClass final {
public:
  [[nodiscard]]
  static auto register_class(WNDCLASSEXW const&) -> Win32WindowClassCPtr;

  Win32WindowClass(HMODULE, ATOM) noexcept;

  Win32WindowClass(Win32WindowClass const&) = delete;
  Win32WindowClass(Win32WindowClass&&) = delete;

  ~Win32WindowClass() noexcept;

  auto operator=(Win32WindowClass const&) -> Win32WindowClass& = delete;
  auto operator=(Win32WindowClass&&) -> Win32WindowClass& = delete;

  [[nodiscard]]
  auto create_window(LPCWSTR name, DWORD style, DWORD styleEx, int x, int y,
                     int width, int height, LPVOID param) const -> HWND;

private:
  HMODULE mModuleHandle;
  ATOM mAtom;
};

} // namespace basalt

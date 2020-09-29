#pragma once

#include "shared/Windows_custom.h"

#include <runtime/input.h>
#include <runtime/types.h>
#include <runtime/shared/size2d.h>

#include <array>
#include <memory>

namespace basalt {

struct Config;

namespace gfx {

struct Device;

} // namespace gfx

struct Window;
using WindowPtr = std::unique_ptr<Window>;

struct Window final {
  Window(const Window&) = delete;
  Window(Window&&) = delete;

  ~Window();

  auto operator=(const Window&) -> Window& = delete;
  auto operator=(Window&&) -> Window& = delete;

  [[nodiscard]]
  auto handle() const -> HWND {
    return mHandle;
  }

  [[nodiscard]]
  auto client_area_size() const -> Size2Du16 {
    return mClientAreaSize;
  }

  void set_cursor(MouseCursor);

  auto drain_input() -> Input;

  [[nodiscard]]
  static auto create(
    HMODULE, int showCommand, const Config& config) -> WindowPtr;

private:
  static constexpr auto CLASS_NAME = L"BS_WINDOW_CLASS";

  HMODULE mModuleHandle {nullptr};
  HWND mHandle {nullptr};

  Input mInput {};
  Size2Du16 mClientAreaSize {Size2Du16::dont_care()};
  bool mInSizingMode {false};

  std::array<HCURSOR, MOUSE_CURSOR_COUNT> mLoadedCursors {};
  MouseCursor mCurrentCursor {};

  Window(HMODULE, HWND, Size2Du16 clientAreaSize);

  [[nodiscard]]
  auto dispatch_message(UINT message, WPARAM, LPARAM) -> LRESULT;

  void process_mouse_message_states(WPARAM);

  static ATOM register_class(HMODULE);

  static auto CALLBACK window_proc(
    HWND, UINT message, WPARAM, LPARAM) -> LRESULT;
};

} // namespace basalt

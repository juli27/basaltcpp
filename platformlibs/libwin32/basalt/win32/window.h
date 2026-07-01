#pragma once

#include "types.h"

#include <basalt/input_manager.h>

#include <basalt/api/shared/types.h>

#include <basalt/win32/shared/Windows_custom.h>

namespace basalt {

class Win32Window {
public:
  Win32Window(Win32Window const&) = delete;
  Win32Window(Win32Window&&) = delete;

  auto operator=(Win32Window const&) -> Win32Window& = delete;
  auto operator=(Win32Window&&) -> Win32Window& = delete;

  [[nodiscard]]
  auto message_queue() const noexcept -> Win32MessageQueue*;

  [[nodiscard]]
  auto handle() const noexcept -> HWND;

  [[nodiscard]]
  auto client_area_size() const noexcept -> Size2Du16;

  [[nodiscard]]
  auto mouse_cursor() const noexcept -> HCURSOR;

  auto set_mouse_cursor(HCURSOR) noexcept -> void;

  [[nodiscard]]
  auto input_manager() noexcept -> InputManager&;

protected:
  Win32Window(HWND, Win32MessageQueue*);

  ~Win32Window() noexcept;

  [[nodiscard]]
  auto handle_message(UINT message, WPARAM, LPARAM) -> LRESULT;

private:
  Win32MessageQueue* mMessageQueue;
  HWND mHandle;
  HCURSOR mMouseCursor;
  InputManager mInputManager;

  [[nodiscard]]
  auto on_set_cursor(HWND windowUnderCursor, WORD hitTestResult,
                     WORD triggerMessage) -> LRESULT;

  [[nodiscard]]
  auto on_keyboard_focus_gained(HWND lostFocus) -> LRESULT;

  [[nodiscard]]
  auto on_keyboard_focus_lost(HWND gainedFocus) -> LRESULT;

  [[nodiscard]]
  auto on_key_down(UINT virtualKeyCode, WORD repeatCount, WORD info) -> LRESULT;

  [[nodiscard]]
  auto on_key_up(UINT virtualKeyCode, WORD repeatCount, WORD info) -> LRESULT;

  [[nodiscard]]
  auto on_char(WCHAR, WORD repeatCount, WORD info) -> LRESULT;

  [[nodiscard]]
  auto on_mouse_move(WORD buttonStates, POINTS pointerPos) -> LRESULT;

  [[nodiscard]]
  auto on_mouse_button(WORD buttonStates, POINTS pointerPos) -> LRESULT;

  [[nodiscard]]
  auto on_x_mouse_button(WORD buttonStates, POINTS pointerPos) -> LRESULT;

  [[nodiscard]]
  auto on_mouse_wheel(SHORT delta, WORD states, POINTS pointerPosScreen)
    -> LRESULT;

  auto process_mouse_button_states(WORD buttonStates) -> void;
};

} // namespace basalt

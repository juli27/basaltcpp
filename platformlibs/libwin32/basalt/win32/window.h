#pragma once

#include <basalt/win32/types.h>

#include <basalt/input_manager.h>

#include <basalt/gfx/types.h>

#include <basalt/win32/shared/Windows_custom.h>

#include <basalt/api/types.h>

#include <basalt/api/shared/size2d.h>
#include <basalt/api/shared/types.h>

#include <array>
#include <string>

namespace basalt {

struct Window final {
  struct Desc {
    std::string title {};
    Size2Du16 preferredClientAreaSize {Size2Du16::dont_care()};
    WindowMode mode {WindowMode::Windowed};
    bool resizeable {true};
  };

  Window(const Window&) = delete;
  Window(Window&&) = delete;

  ~Window();

  auto operator=(const Window&) -> Window& = delete;
  auto operator=(Window&&) -> Window& = delete;

  [[nodiscard]] auto handle() const noexcept -> HWND;
  [[nodiscard]] auto input_manager() noexcept -> InputManager&;
  [[nodiscard]] auto client_area_size() const noexcept -> Size2Du16;
  [[nodiscard]] auto current_mode() const noexcept -> WindowMode;

  void set_mode(WindowMode);
  void set_cursor(MouseCursor) noexcept;

  // return null on failure
  [[nodiscard]] static auto create(HMODULE, int showCommand, const Desc&)
    -> WindowPtr;

private:
  struct SavedWindowInfo final {
    DWORD style {};
    RECT rect {};
  };

  static constexpr auto CLASS_NAME = L"BS_WINDOW_CLASS";

  HMODULE mModuleHandle {};
  HWND mHandle {};

  InputManager mInputManager;

  SavedWindowInfo mSavedWindowInfo;
  // set on first WM_SIZE
  Size2Du16 mClientAreaSize {Size2Du16::dont_care()};

  WindowMode mCurrentMode {WindowMode::Windowed};
  MouseCursor mCurrentCursor {MouseCursor::Arrow};

  std::array<HCURSOR, MOUSE_CURSOR_COUNT> mLoadedCursors {};

  Window(HMODULE, HWND);

  [[nodiscard]] auto handle_message(UINT message, WPARAM, LPARAM) -> LRESULT;

  void process_mouse_message_states(WPARAM);

  void update_client_area_size();

  static ATOM register_class(HMODULE);

  static auto CALLBACK window_proc(HWND, UINT message, WPARAM, LPARAM)
    -> LRESULT;
};

} // namespace basalt

#pragma once

#include <basalt/win32/types.h>

#include <basalt/win32/shared/types.h>
#include <basalt/win32/shared/Windows_custom.h>

#include <basalt/input_manager.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/shared/size2d.h>
#include <basalt/api/shared/types.h>

#include <array>
#include <string>

namespace basalt {

class Window final {
public:
  struct CreateInfo final {
    std::string title;
    int showCommand;
    Size2Du16 preferredClientAreaSize {Size2Du16::dont_care()};
    WindowMode mode {WindowMode::Windowed};
    bool resizeable {true};
  };

  // throws std::system_error on failure
  [[nodiscard]] static auto create(HMODULE, const CreateInfo&,
                                   const gfx::Win32GfxFactory&) -> WindowPtr;

  Window(const Window&) = delete;
  Window(Window&&) = delete;

  ~Window();

  auto operator=(const Window&) -> Window& = delete;
  auto operator=(Window&&) -> Window& = delete;

  [[nodiscard]] auto handle() const noexcept -> HWND;
  [[nodiscard]] auto gfx_context() const noexcept -> const gfx::ContextPtr&;
  [[nodiscard]] auto input_manager() noexcept -> InputManager&;
  [[nodiscard]] auto client_area_size() const noexcept -> Size2Du16;
  [[nodiscard]] auto mode() const noexcept -> WindowMode;

  auto set_mode(WindowMode) -> void;
  auto set_cursor(MouseCursor) noexcept -> void;

  // don't call directly. Use the create function instead
  Window(HMODULE, ATOM classAtom, HWND, Size2Du16 clientAreaSize,
         const gfx::AdapterList&);

private:
  struct SavedWindowInfo final {
    DWORD style {};
    RECT windowRect {}; // in screen coordinates
  };

  HMODULE mModuleHandle {};
  ATOM mClassAtom {};
  HWND mHandle {};
  const gfx::AdapterList& mAdapters;
  gfx::ContextPtr mGfxContext;
  InputManager mInputManager;
  SavedWindowInfo mSavedWindowInfo;
  Size2Du16 mClientAreaSize {Size2Du16::dont_care()};
  WindowMode mCurrentMode {WindowMode::Windowed};
  MouseCursor mCurrentCursor {MouseCursor::Arrow};
  std::array<HCURSOR, MOUSE_CURSOR_COUNT> mLoadedCursors {};
  bool mIsInSizeMoveModalLoop {false};

  auto init_gfx_context(const gfx::Win32GfxFactory&) -> void;
  auto shutdown_gfx_context() -> void;

  auto resize(Size2Du16 newClientAreaSize) -> void;

  [[nodiscard]] auto handle_message(UINT message, WPARAM, LPARAM) -> LRESULT;
  auto on_size(WPARAM resizeType, Size2Du16 newClientAreaSize) -> void;
  [[nodiscard]] auto on_keyboard_focus(UINT message, HWND other) -> LRESULT;
  [[nodiscard]] auto on_close() -> LRESULT;
  [[nodiscard]] auto on_set_cursor(HWND windowUnderCursor, SHORT hitTestResult,
                                   USHORT triggerMessage) -> LRESULT;
  [[nodiscard]] auto on_key(WPARAM virtualKeyCode, WORD repeatCount, WORD info)
    -> LRESULT;
  [[nodiscard]] auto on_char(WPARAM characterCode, WORD repeatCount, WORD info)
    -> LRESULT;
  [[nodiscard]] auto on_mouse_move(WPARAM, PointerPosition) -> LRESULT;
  [[nodiscard]] auto on_mouse_button(UINT message, WPARAM, PointerPosition)
    -> LRESULT;
  [[nodiscard]] auto on_mouse_wheel(SHORT delta, WPARAM states, PointerPosition)
    -> LRESULT;

  auto process_mouse_message_states(WPARAM) -> void;

  static auto CALLBACK window_proc(HWND, UINT message, WPARAM, LPARAM)
    -> LRESULT;
};

} // namespace basalt

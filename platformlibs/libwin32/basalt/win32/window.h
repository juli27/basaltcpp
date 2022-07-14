#pragma once

#include <basalt/win32/types.h>

#include <basalt/win32/shared/Windows_custom.h>

#include <basalt/input_manager.h>

#include <basalt/gfx/backend/d3d9/types.h>

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

  // throws std::system_error, return null on failure
  [[nodiscard]] static auto create(HMODULE, const CreateInfo&,
                                   const gfx::D3D9Factory&) -> WindowPtr;

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
  Window(HMODULE, ATOM classAtom, Size2Du16 clientAreaSize,
         const gfx::AdapterList&);

private:
  struct SavedWindowInfo final {
    DWORD style {};
    RECT windowRect {}; // in screen coordinates
  };

  HMODULE mModuleHandle {};
  ATOM mClassAtom {};

  // set during WM_CREATE in the window_proc
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

  auto init_gfx_context(const gfx::D3D9Factory&) -> void;
  auto shutdown_gfx_context() -> void;

  [[nodiscard]] auto handle_message(UINT message, WPARAM, LPARAM) -> LRESULT;
  auto on_create(const CREATESTRUCTW&) const -> void;
  auto on_resize(Size2Du16 newClientAreaSize) -> void;

  auto process_mouse_message_states(WPARAM) -> void;

  static auto register_class(HMODULE) -> ATOM;

  static auto CALLBACK window_proc(HWND, UINT message, WPARAM, LPARAM)
    -> LRESULT;
};

} // namespace basalt

#pragma once

#include <basalt/win32/types.h>

#include <basalt/win32/shared/Windows_custom.h>

#include <basalt/input_manager.h>

#include <basalt/gfx/backend/d3d9/types.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/types.h>

#include <basalt/api/shared/size2d.h>
#include <basalt/api/shared/types.h>

#include <array>
#include <string>

namespace basalt {

struct Window final {
private:
  // disallow outside construction while enabling make_unique
  struct Token {};

public:
  struct Desc {
    std::string title {};
    Size2Du16 preferredClientAreaSize {Size2Du16::dont_care()};
    WindowMode mode {WindowMode::Windowed};
    bool resizeable {true};
  };

  Window(Token, HMODULE, Size2Du16 clientAreaSize);

  Window(const Window&) = delete;
  Window(Window&&) = delete;

  ~Window();

  auto operator=(const Window&) -> Window& = delete;
  auto operator=(Window&&) -> Window& = delete;

  [[nodiscard]] auto handle() const noexcept -> HWND;
  [[nodiscard]] auto gfx_context() const noexcept -> gfx::Context&;
  [[nodiscard]] auto input_manager() noexcept -> InputManager&;
  [[nodiscard]] auto client_area_size() const noexcept -> Size2Du16;
  [[nodiscard]] auto mode() const noexcept -> WindowMode;

  void set_mode(WindowMode);
  void set_cursor(MouseCursor) noexcept;

  // return null on failure
  [[nodiscard]] static auto create(HMODULE, int showCommand, const Desc&,
                                   const gfx::D3D9Factory&) -> WindowPtr;

private:
  struct SavedWindowInfo final {
    DWORD style {};
    RECT windowRect {}; // in screen coordinates
  };

  static constexpr auto CLASS_NAME = L"BasaltWindow";

  HMODULE mModuleHandle {};
  // set during WM_CREATE in the window_proc
  HWND mHandle {};

  gfx::ContextPtr mGfxContext {};

  InputManager mInputManager;

  SavedWindowInfo mSavedWindowInfo;
  Size2Du16 mClientAreaSize {Size2Du16::dont_care()};

  WindowMode mCurrentMode {WindowMode::Windowed};
  MouseCursor mCurrentCursor {MouseCursor::Arrow};

  std::array<HCURSOR, MOUSE_CURSOR_COUNT> mLoadedCursors {};

  bool mIsInSizeMoveModalLoop {false};

  void init_gfx_context(const gfx::D3D9Factory&);
  void shutdown_gfx_context();

  [[nodiscard]] auto handle_message(UINT message, WPARAM, LPARAM) -> LRESULT;
  void on_create(const CREATESTRUCTW&) const;
  void on_resize(Size2Du16 newClientAreaSize);

  void process_mouse_message_states(WPARAM);

  static ATOM register_class(HMODULE);

  static auto CALLBACK window_proc(HWND, UINT message, WPARAM, LPARAM)
    -> LRESULT;
};

} // namespace basalt

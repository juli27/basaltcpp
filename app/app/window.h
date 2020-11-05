#pragma once

#include "types.h"

#include "gfx/types.h"

#include "shared/Windows_custom.h"

#include <api/input.h>
#include <api/types.h>

#include <api/gfx/backend/types.h>

#include <api/shared/size2d.h>
#include <api/shared/types.h>

#include <array>

namespace basalt {

struct Window final {
  Window(const Window&) = delete;
  Window(Window&&) = delete;

  ~Window();

  auto operator=(const Window&) -> Window& = delete;
  auto operator=(Window &&) -> Window& = delete;

  [[nodiscard]] auto handle() const noexcept -> HWND;
  [[nodiscard]] auto client_area_size() const noexcept -> Size2Du16;
  [[nodiscard]] auto current_mode() const noexcept -> WindowMode;

  void set_mode(WindowMode, const gfx::AdapterMode&);
  void set_cursor(MouseCursor) noexcept;
  auto drain_input() -> Input;

  // return null on failure
  [[nodiscard]] static auto create(HMODULE, int showCommand,
                                   const Config& config,
                                   const gfx::AdapterMode&) -> WindowPtr;

private:
  struct SavedWindowInfo final {
    DWORD style {};
    RECT rect {};
  };

  static constexpr auto CLASS_NAME = L"BS_WINDOW_CLASS";

  HMODULE mModuleHandle {};
  HWND mHandle {};

  Input mInput;

  SavedWindowInfo mSavedWindowInfo;
  // set on first WM_SIZE
  Size2Du16 mClientAreaSize {Size2Du16::dont_care()};
  bool mInSizingMode {false};

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

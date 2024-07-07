#pragma once

#include "window.h"

#include "types.h"

#include <basalt/win32/shared/types.h>

#include <basalt/api/bootstrap.h>
#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/shared/types.h>

#include <basalt/win32/shared/Windows_custom.h>

namespace basalt {

class Win32AppWindow final : public Win32Window {
public:
  // throws std::system_error on failure
  [[nodiscard]]
  static auto create(HMODULE, int const showCommand, AppLaunchInfo const&)
    -> Win32AppWindowPtr;

  // don't call directly. Use the create function instead
  Win32AppWindow(HWND, Win32WindowClassCPtr, Win32MessageQueuePtr,
                 gfx::Win32GfxFactoryPtr);

  Win32AppWindow(Win32AppWindow const&) = delete;
  Win32AppWindow(Win32AppWindow&&) = delete;

  ~Win32AppWindow();

  auto operator=(Win32AppWindow const&) -> Win32AppWindow& = delete;
  auto operator=(Win32AppWindow&&) -> Win32AppWindow& = delete;

  [[nodiscard]]
  auto gfx_context() const noexcept -> gfx::ContextPtr const&;

  [[nodiscard]]
  auto is_fullscreen() const noexcept -> bool;

  [[nodiscard]]
  auto mode() const noexcept -> WindowMode;

  auto set_mode(WindowMode) -> void;

  auto present() const -> gfx::PresentResult;

private:
  struct SavedWindowInfo final {
    DWORD style{};
    RECT windowRect{}; // in screen coordinates
  };

  WNDPROC mSuperClassWndProc;
  gfx::Win32GfxFactoryPtr mGfxFactory;
  gfx::ContextPtr mGfxContext;
  gfx::SwapChainPtr mSwapChain;
  SavedWindowInfo mSavedWindowInfo;
  WindowMode mMode{WindowMode::Windowed};
  bool mIsInSizeMoveModalLoop{false};

  auto init_gfx_context(CanvasCreateInfo const&, gfx::Win32GfxFactory const&)
    -> void;

  auto make_fullscreen() -> void;

  auto make_windowed() -> void;

  auto call_super_class_wnd_proc(UINT message, WPARAM, LPARAM) const noexcept
    -> LRESULT;

  [[nodiscard]]
  auto handle_message(UINT message, WPARAM, LPARAM) -> LRESULT;

  auto on_size(Size2Du16 newClientAreaSize) -> void;

  [[nodiscard]]
  auto on_size(WPARAM resizeType, Size2Du16 newClientAreaSize) -> LRESULT;

  [[nodiscard]]
  auto on_enter_size_move() -> LRESULT;

  [[nodiscard]]
  auto on_exit_size_move() -> LRESULT;

  [[nodiscard]]
  auto on_close() -> LRESULT;

  static auto instance(HWND) -> Win32AppWindow*;

  static auto CALLBACK wnd_proc(HWND, UINT message, WPARAM, LPARAM) -> LRESULT;
};

} // namespace basalt

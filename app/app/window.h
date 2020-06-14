#pragma once

#include "d3d9/context.h"
#include "d3d9/factory.h"
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

namespace win32 {

struct Window;
using WindowPtr = std::unique_ptr<Window>;

struct Window final {
  Window() = delete;

  Window(const Window&) = delete;
  Window(Window&&) = delete;

  ~Window();

  auto operator=(const Window&) -> Window& = delete;
  auto operator=(Window&&) -> Window& = delete;

  [[nodiscard]]
  auto size() const -> Size2Du16 {
    return mClientAreaSize;
  }

  [[nodiscard]]
  auto handle() const -> HWND {
    return mHandle;
  }

  [[nodiscard]]
  auto context_factory() const -> const gfx::backend::D3D9FactoryPtr& {
    return mFactory;
  }

  [[nodiscard]]
  auto gfx_context() const -> std::shared_ptr<gfx::Context> {
    return mGfxContext;
  }

  [[nodiscard]]
  auto gfx_device() const -> gfx::Device* {
    return &mGfxContext->device();
  }

  void set_cursor(MouseCursor);

  auto drain_input() -> Input;

  void present() const {
    mGfxContext->present();
  }

  [[nodiscard]]
  static auto create(
    HMODULE, int showCommand, const Config& config) -> WindowPtr;

private:
  static constexpr auto CLASS_NAME = L"BS_WINDOW_CLASS";

  HMODULE mModuleHandle {nullptr};
  HWND mHandle {nullptr};

  gfx::backend::D3D9FactoryPtr mFactory {};
  std::shared_ptr<gfx::backend::D3D9Context> mGfxContext {};

  Input mInput {};
  Size2Du16 mClientAreaSize {Size2Du16::dont_care()};
  bool mInSizingMode {false};

  std::array<HCURSOR, MOUSE_CURSOR_COUNT> mLoadedCursors {};
  MouseCursor mCurrentCursor {};

  Window(
    HMODULE, HWND, gfx::backend::D3D9FactoryPtr
  , std::shared_ptr<gfx::backend::D3D9Context>, Size2Du16 clientAreaSize);

  [[nodiscard]]
  auto dispatch_message(UINT message, WPARAM, LPARAM) -> LRESULT;

  void do_resize(Size2Du16 clientArea) const;

  void process_mouse_message_states(WPARAM);

  static ATOM register_class(HMODULE);

  static auto CALLBACK window_proc(
    HWND, UINT message, WPARAM, LPARAM) -> LRESULT;
};

} // namespace win32
} // namespace basalt

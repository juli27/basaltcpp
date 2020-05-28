#pragma once
#ifndef BASALT_APP_WINDOW_H
#define BASALT_APP_WINDOW_H

#include "d3d9/context.h"
#include "d3d9/context_factory.h"
#include "shared/Windows_custom.h"

#include <runtime/Input.h>
#include <runtime/shared/Size2D.h>

#include <memory>

namespace basalt {

struct Config;

namespace gfx::backend {

struct IRenderer;

} // namespace gfx::backend

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
  auto context_factory() const -> const gfx::backend::D3D9ContextFactoryPtr& {
    return mFactory;
  }

  [[nodiscard]]
  auto renderer() const -> gfx::backend::IRenderer* {
    return mContext->renderer().get();
  }

  auto drain_input() -> Input;

  void present() const {
    mContext->present();
  }

  [[nodiscard]]
  static auto create(
    HMODULE, int showCommand, const Config& config) -> WindowPtr;

private:
  static constexpr auto CLASS_NAME = L"BS_WINDOW_CLASS";

  HMODULE mModuleHandle {nullptr};
  HWND mHandle {nullptr};

  gfx::backend::D3D9ContextFactoryPtr mFactory {};
  std::unique_ptr<gfx::backend::D3D9GfxContext> mContext {};

  Input mInput {};
  Size2Du16 mClientAreaSize {Size2Du16::dont_care()};
  bool mInSizingMode {false};

  Window(
    HMODULE, HWND handle, gfx::backend::D3D9ContextFactoryPtr factory
  , std::unique_ptr<gfx::backend::D3D9GfxContext> context
  , Size2Du16 clientAreaSize);

  [[nodiscard]]
  auto dispatch_message(UINT message, WPARAM, LPARAM) -> LRESULT;

  void do_resize(Size2Du16 clientArea) const;

  static ATOM register_class(HMODULE);

  static auto CALLBACK window_proc(
    HWND, UINT message, WPARAM, LPARAM) -> LRESULT;
};

} // namespace win32
} // namespace basalt

#endif // BASALT_APP_WINDOW_H

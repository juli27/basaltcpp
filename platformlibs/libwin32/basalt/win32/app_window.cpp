#include "app_window.h"

#include "message_queue.h"
#include "util.h"
#include "window_class.h"

#include <basalt/gfx/backend/d3d9/factory.h>

#include <basalt/win32/shared/utils.h>
#include <basalt/win32/shared/win32_gfx_factory.h>

#include <basalt/api/gfx/context.h>

#include <basalt/gfx/backend/swap_chain.h>

#include <basalt/api/base/asserts.h>
#include <basalt/api/base/log.h>
#include <basalt/api/base/utils.h>

#include <basalt/win32/shared/Windows_custom.h>
#include <windowsx.h>

#include <algorithm>
#include <optional>
#include <string>
#include <system_error>
#include <utility>
#include <variant>

using namespace std::literals;

using std::nullopt;
using std::optional;
using std::system_error;

namespace basalt {

namespace {

struct CreateParams final {
  Size2Du16 clientAreaSize;
};

[[nodiscard]]
auto create_gfx_factory(gfx::BackendApi const backendApi)
  -> optional<gfx::Win32GfxFactoryPtr> {
  switch (backendApi) {
  case gfx::BackendApi::Default:
  case gfx::BackendApi::Direct3D9:
    return gfx::D3D9Factory::create();

  default:
    BASALT_LOG_ERROR("win32: no suitable graphics API available");

    return nullopt;
  }
}

// posX and posY: location of the upper left corner of the client area
// clientArea is the preferred size of the client area (width and/or height can
// be 0 if no preference)
// workArea in virtual-screen coords
auto calc_window_rect(int const posX, int const posY, DWORD const style,
                      DWORD const styleEx, Size2Du16 const clientArea,
                      RECT const& workArea) noexcept -> RECT {
  // window dimensions in client coords
  auto rect = RECT{0l, 0l, clientArea.width(), clientArea.height()};

  OffsetRect(&rect, posX, posY);

  // calculate the window size for the given client area size
  AdjustWindowRectEx(&rect, style, FALSE, styleEx);

  if (rect.right > workArea.right) {
    OffsetRect(&rect, -std::min(rect.right - workArea.right, rect.left), 0);
  }

  if (rect.bottom > workArea.bottom) {
    OffsetRect(&rect, 0, -std::min(rect.bottom - workArea.bottom, rect.top));
  }

  if (rect.right > workArea.right) {
    rect.right += workArea.right - rect.right;
  }

  if (rect.bottom > workArea.bottom) {
    rect.bottom += workArea.bottom - rect.bottom;
  }

  return rect;
}

auto CALLBACK bootstrap_proc(HWND const handle, UINT const message,
                             WPARAM const wParam,
                             LPARAM const lParam) noexcept -> LRESULT {
  if (message == WM_CREATE) {
    auto const* cs = reinterpret_cast<CREATESTRUCTW const*>(lParam);
    auto const* const createParams =
      static_cast<CreateParams const*>(cs->lpCreateParams);

    auto const monitor = MonitorFromWindow(handle, MONITOR_DEFAULTTONEAREST);
    auto mi = MONITORINFO{};
    mi.cbSize = sizeof(MONITORINFO);
    GetMonitorInfoW(monitor, &mi);

    auto topLeftClient = POINT{0, 0};
    ClientToScreen(handle, &topLeftClient);

    auto const rect =
      calc_window_rect(topLeftClient.x, topLeftClient.y, cs->style,
                       cs->dwExStyle, createParams->clientAreaSize, mi.rcWork);

    SetWindowPos(handle, nullptr, rect.left, rect.top, rect.right - rect.left,
                 rect.bottom - rect.top, SWP_NOZORDER | SWP_NOACTIVATE);
  }

  return DefWindowProcW(handle, message, wParam, lParam);
}

struct Win32WindowStyle {
  DWORD style;
  DWORD styleEx;
};

[[nodiscard]]
auto get_style_windowed(bool const isUserResizeable) -> Win32WindowStyle {
  // WS_CLIPSIBLINGS is added automatically (tested on Windows 10)
  auto style = DWORD{WS_OVERLAPPEDWINDOW};
  if (!isUserResizeable) {
    style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
  }

  // WS_EX_WINDOWEDGE is added automatically (tested on Windows 10)
  constexpr auto styleEx = DWORD{};

  return Win32WindowStyle{style, styleEx};
}

auto register_class(HMODULE const moduleHandle) -> Win32WindowClassCPtr {
  constexpr auto className = L"BasaltWindow";

  auto const windowClass = WNDCLASSEXW{
    sizeof(WNDCLASSEXW),
    0, // style
    &bootstrap_proc,
    0, // cbClsExtra
    0, // cbWndExtra
    moduleHandle,
    nullptr, // hIcon
    nullptr, // hCursor
    GetSysColorBrush(COLOR_WINDOW), // TODO: is the background brush needed?
    nullptr, // lpszMenuName
    className,
    nullptr, // hIconSm
  };

  return Win32WindowClass::register_class(windowClass);
}

auto get_default_gfx_context_info(gfx::AdapterInfos const& adapters)
  -> GfxContextCreateInfo {
  auto const& adapterInfo = adapters[0];
  auto const backBufferFormat = [&] {
    for (auto const& format : adapterInfo.sharedModeInfo.backBufferFormats) {
      if (format.renderTargetFormat == gfx::ImageFormat::B8G8R8A8) {
        return format;
      }
      if (format.renderTargetFormat == gfx::ImageFormat::B8G8R8X8) {
        return format;
      }
    }

    return adapterInfo.sharedModeInfo.backBufferFormats[0];
  }();

  return GfxContextCreateInfo{
    0,
    backBufferFormat.renderTargetFormat,
    backBufferFormat.depthStencilFormat,
    gfx::MultiSampleCount::One,
  };
}

} // namespace

auto Win32AppWindow::create(HMODULE const moduleHandle, int const showCommand,
                            AppLaunchInfo const& app) -> Win32AppWindowPtr {
  auto const& canvasInfo = app.canvasCreateInfo;
  auto const gfxFactory = [&] {
    auto maybeFactory = create_gfx_factory(canvasInfo.gfxBackendApi);
    if (!maybeFactory) {
      BASALT_CRASH("win32: couldn't create a gfx factory");
    }

    return std::move(maybeFactory).value();
  }();

  auto const adapters = gfxFactory->enumerate_adapters();

  auto gfxContextInfo = canvasInfo.configureGfxContext
                          ? canvasInfo.configureGfxContext(adapters)
                          : get_default_gfx_context_info(adapters);

  // the default size is two thirds of the current display mode
  auto const windowSize = [&] {
    auto const& sharedModeInfo =
      adapters[gfxContextInfo.adapter].sharedModeInfo;
    auto const& displayMode = sharedModeInfo.displayMode;
    auto size = canvasInfo.size;
    if (size.width() == 0) {
      size.set_width(static_cast<u16>(MulDiv(displayMode.width, 2, 3)));
    }
    if (size.height() == 0) {
      size.set_height(static_cast<u16>(MulDiv(displayMode.height, 2, 3)));
    }

    return size;
  }();

  auto const [style, styleEx] = get_style_windowed(canvasInfo.isUserResizeable);
  auto const windowTitle = create_wide_from_utf8(app.appName);
  auto params = CreateParams{windowSize};

  auto windowClass = register_class(moduleHandle);
  if (!windowClass) {
    throw system_error{static_cast<int>(GetLastError()), std::system_category(),
                       "Failed to register window class"s};
  }

  auto const handle =
    windowClass->create_window(windowTitle.c_str(), style, styleEx,
                               CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, &params);
  if (!handle) {
    throw system_error{static_cast<int>(GetLastError()), std::system_category(),
                       "Failed to create window"s};
  }

  auto window = std::make_unique<Win32AppWindow>(
    handle, std::move(windowClass), Win32MessageQueue::get_for_current_thread(),
    gfxFactory);

  ShowWindow(handle, showCommand);

  // required to be after ShowWindow because otherwise the application icon
  // would not appear in the titlebar if the application launches with
  // fullscreen and switches to windowed
  // TODO: find a better workaround
  window->set_mode(canvasInfo.mode);

  window->init_gfx_context(gfxContextInfo, *gfxFactory);

  auto const& adapterIdentifier = adapters[gfxContextInfo.adapter].identifier;

  BASALT_LOG_INFO("Direct3D9 context created: adapter={}, driver={}",
                  adapterIdentifier.displayName, adapterIdentifier.driverInfo);

  return window;
}

Win32AppWindow::Win32AppWindow(HWND const handle,
                               Win32WindowClassCPtr windowClass,
                               Win32MessageQueuePtr messageQueue,
                               gfx::Win32GfxFactoryPtr gfxFactory)
  : Win32Window{handle, std::move(windowClass), std::move(messageQueue)}
  , mGfxFactory{std::move(gfxFactory)} {
  BASALT_ASSERT(mGfxFactory);

  // safe because the OS window data is destroyed by the destructor and moving
  // window objects is prohibited
  SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

  // subclass window
  auto const superClassWndProc = SetWindowLongPtrW(
    handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&wnd_proc));
  mSuperClassWndProc = reinterpret_cast<WNDPROC>(superClassWndProc);
}

Win32AppWindow::~Win32AppWindow() = default;

auto Win32AppWindow::gfx_context() const noexcept -> gfx::ContextPtr const& {
  return mGfxContext;
}

auto Win32AppWindow::is_fullscreen() const noexcept -> bool {
  return mMode == WindowMode::Fullscreen ||
         mMode == WindowMode::FullscreenExclusive;
}

auto Win32AppWindow::mode() const noexcept -> WindowMode {
  return mMode;
}

auto Win32AppWindow::set_mode(WindowMode const newMode) -> void {
  if (newMode == mMode) {
    return;
  }

  // exclusive ownership of the output monitor needs to be released before
  // window changes can be made
  // is null when called before init_gfx_context
  if (mSwapChain) {
    if (auto info = mSwapChain->get_info(); info.is_exclusive()) {
      info.modeInfo = gfx::SwapChain::SharedModeInfo{client_area_size()};
      mSwapChain->reset(info);

      // the d3d9 runtime leaves the window as topmost when exiting exclusive
      // fullscreen
      SetWindowPos(handle(), HWND_NOTOPMOST, 0, 0, 0, 0,
                   SWP_NOSIZE | SWP_NOSIZE | SWP_NOACTIVATE);

      mMode = WindowMode::Fullscreen;
    }
  }

  switch (newMode) {
  case WindowMode::Windowed:
    make_windowed();

    break;

  case WindowMode::Fullscreen:
    make_fullscreen();

    break;

  case WindowMode::FullscreenExclusive: {
    make_fullscreen();
    mMode = WindowMode::FullscreenExclusive;

    // is null when called before init_gfx_context
    if (mSwapChain) {
      auto swapChainInfo = mSwapChain->get_info();
      swapChainInfo.modeInfo =
        gfx::SwapChain::ExclusiveModeInfo{mExclusiveDisplayMode.value()};
      mSwapChain->reset(swapChainInfo);
    }

    break;
  }
  }
}

auto Win32AppWindow::present() const -> gfx::PresentResult {
  return mSwapChain->present();
}

auto Win32AppWindow::init_gfx_context(GfxContextCreateInfo const& createInfo,
                                      gfx::Win32GfxFactory const& gfxFactory)
  -> void {
  auto const modeInfo =
    mMode == WindowMode::FullscreenExclusive
      ? gfx::SwapChain::ModeInfo{gfx::SwapChain::ExclusiveModeInfo{
          createInfo.exclusiveDisplayMode.value()}}
      : gfx::SwapChain::ModeInfo{
          gfx::SwapChain::SharedModeInfo{client_area_size()}};
  auto const swapChainInfo = gfx::SwapChain::Info{
    modeInfo,
    createInfo.colorFormat,
    createInfo.depthStencilFormat,
    createInfo.sampleCount,
  };

  mGfxContext =
    gfxFactory.create_context(handle(), createInfo.adapter, swapChainInfo);
  mSwapChain = mGfxContext->swap_chain();
  mExclusiveDisplayMode = createInfo.exclusiveDisplayMode;
}

auto Win32AppWindow::make_fullscreen() -> void {
  if (is_fullscreen()) {
    return;
  }

  mMode = WindowMode::Fullscreen;

  auto style = static_cast<DWORD>(GetWindowLongPtrW(handle(), GWL_STYLE));

  mSavedWindowInfo.style = style;
  GetWindowRect(handle(), &mSavedWindowInfo.windowRect);

  auto windowRect = RECT{};
  auto monitorInfo = MONITORINFO{};
  monitorInfo.cbSize = sizeof(MONITORINFO);
  GetMonitorInfoW(
    MonitorFromRect(&mSavedWindowInfo.windowRect, MONITOR_DEFAULTTONEAREST),
    &monitorInfo);

  windowRect = monitorInfo.rcMonitor;
  style &= ~WS_OVERLAPPEDWINDOW;

  SetWindowLongPtrW(handle(), GWL_STYLE, style);

  // SWP_NOCOPYBITS causes the window to flash white
  constexpr auto swpFlags =
    UINT{SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED};
  SetWindowPos(handle(), nullptr, windowRect.left, windowRect.top,
               windowRect.right - windowRect.left,
               windowRect.bottom - windowRect.top, swpFlags);
}

auto Win32AppWindow::make_windowed() -> void {
  BASALT_ASSERT(
    mMode != WindowMode::FullscreenExclusive,
    "fullscreen exclusive mode must be handled by the gfx context first");

  mMode = WindowMode::Windowed;

  auto const style = mSavedWindowInfo.style;
  auto rect = mSavedWindowInfo.windowRect;

  SetWindowLongPtrW(handle(), GWL_STYLE, style);

  // SWP_NOCOPYBITS causes the window to flash white
  constexpr auto swpFlags =
    UINT{SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED};
  SetWindowPos(handle(), nullptr, rect.left, rect.top, rect.right - rect.left,
               rect.bottom - rect.top, swpFlags);
}

auto Win32AppWindow::call_super_class_wnd_proc(
  UINT const message, WPARAM const wParam,
  LPARAM const lParam) const noexcept -> LRESULT {
  return CallWindowProcW(mSuperClassWndProc, handle(), message, wParam, lParam);
}

auto Win32AppWindow::handle_message(UINT const message, WPARAM const wParam,
                                    LPARAM const lParam) -> LRESULT {
  switch (message) {
  case WM_SIZE:
    return on_size(wParam, Size2Du16{LOWORD(lParam), HIWORD(lParam)});

  case WM_ENTERSIZEMOVE:
    return on_enter_size_move();

  case WM_EXITSIZEMOVE:
    return on_exit_size_move();

  case WM_CLOSE:
    return on_close();

  default:
    break;
  }

  return call_super_class_wnd_proc(message, wParam, lParam);
}

auto Win32AppWindow::on_size(Size2Du16 const newClientAreaSize) -> void {
  // mSwapChain is null when this method is called from on_create through
  // SetWindowPos
  if (mSwapChain) {
    auto swapChainInfo = mSwapChain->get_info();
    if (auto* sharedModeInfo = std::get_if<gfx::SwapChain::SharedModeInfo>(
          &swapChainInfo.modeInfo)) {
      auto const currentSize = swapChainInfo.size();

      if (newClientAreaSize != currentSize) {
        sharedModeInfo->size = newClientAreaSize;
        mSwapChain->reset(swapChainInfo);
      }
    }
  }
}

auto Win32AppWindow::on_size(WPARAM const resizeType,
                             Size2Du16 const newClientAreaSize) -> LRESULT {
  switch (resizeType) {
  case SIZE_RESTORED:
  case SIZE_MAXIMIZED:
    if (!mIsInSizeMoveModalLoop) {
      on_size(newClientAreaSize);
    }
    break;

  default:
    break;
  }

  return 0;
}

auto Win32AppWindow::on_enter_size_move() -> LRESULT {
  mIsInSizeMoveModalLoop = true;

  return 0;
}

auto Win32AppWindow::on_exit_size_move() -> LRESULT {
  mIsInSizeMoveModalLoop = false;

  on_size(client_area_size());

  return 0;
}

// TODO: propagate event
auto Win32AppWindow::on_close() -> LRESULT {
  // DefWindowProcW would destroy the window. This would invalidate the window
  // handle and therefore put this Window object in an invalid state. In order
  // to prevent this, we handle the WM_CLOSE message here to trigger our
  // regular shutdown path
  PostQuitMessage(0);

  return 0;
}

auto Win32AppWindow::instance(HWND const handle) -> Win32AppWindow* {
  auto const userData = GetWindowLongPtrW(handle, GWLP_USERDATA);

  return reinterpret_cast<Win32AppWindow*>(userData);
}

auto Win32AppWindow::wnd_proc(HWND const handle, UINT const message,
                              WPARAM const wParam,
                              LPARAM const lParam) -> LRESULT {
  auto* const window = instance(handle);
  BASALT_ASSERT(window);

  return window->handle_message(message, wParam, lParam);
}

} // namespace basalt

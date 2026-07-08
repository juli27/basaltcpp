#include "app_window.h"

#include "resources.h"
#include "util.h"

#include "shared/utils.h"
#include "shared/win32_gfx_factory.h"

#include <basalt/api/gfx/context.h>

#include <basalt/gfx/backend/swap_chain.h>

#include <basalt/api/base/asserts.h>
#include <basalt/api/base/log.h>

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>

namespace basalt {

namespace {

struct CreateParams {
  std::optional<Size2Du16> clientAreaSize;
};

// posX and posY: location of the upper left corner of the client area
// clientArea is the preferred size of the client area
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

auto get_default_client_area_size(MONITORINFO const& monitorInfo) -> Size2Du16 {
  auto const monitorSize = win32::get_size_u16(monitorInfo.rcMonitor);
  auto const width = MulDiv(monitorSize.width(), 2, 3);
  auto const height = MulDiv(monitorSize.height(), 2, 3);

  return Size2Du16{static_cast<u16>(width), static_cast<u16>(height)};
}

auto CALLBACK handle_create_message(HWND const handle, UINT const messageId,
                                    WPARAM const wParam,
                                    LPARAM const lParam) noexcept -> LRESULT {
  if (messageId == WM_CREATE) {
    auto const monitorInfo = [&] {
      auto const monitor = MonitorFromWindow(handle, MONITOR_DEFAULTTONEAREST);

      auto info = MONITORINFO{};
      info.cbSize = sizeof(info);
      GetMonitorInfoW(monitor, &info);

      return info;
    }();

    auto const* cs = reinterpret_cast<CREATESTRUCTW const*>(lParam);

    auto const clientAreaSize = [&] {
      auto const* createParams =
        static_cast<CreateParams const*>(cs->lpCreateParams);

      return createParams->clientAreaSize
               ? *createParams->clientAreaSize
               : get_default_client_area_size(monitorInfo);
    }();

    auto clientAreaPosition = POINT{0, 0};
    ClientToScreen(handle, &clientAreaPosition);

    auto const rect =
      calc_window_rect(clientAreaPosition.x, clientAreaPosition.y, cs->style,
                       cs->dwExStyle, clientAreaSize, monitorInfo.rcWork);

    SetWindowPos(handle, nullptr, rect.left, rect.top, rect.right - rect.left,
                 rect.bottom - rect.top, SWP_NOZORDER | SWP_NOACTIVATE);
  }

  return DefWindowProcW(handle, messageId, wParam, lParam);
}

struct Win32WindowStyle {
  DWORD style;
  DWORD styleEx;
};

[[nodiscard]]
auto get_style_windowed(bool const isUserResizeable) -> Win32WindowStyle {
  auto style = DWORD{WS_OVERLAPPEDWINDOW};
  if (!isUserResizeable) {
    style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
  }

  auto constexpr styleEx = DWORD{WS_EX_LEFT | WS_EX_LTRREADING};

  return Win32WindowStyle{style, styleEx};
}

} // namespace

auto Win32AppWindow::create(HMODULE const moduleHandle,
                            Win32MessageQueue* messageQueue,
                            std::wstring const& title,
                            gfx::Win32GfxFactoryPtr gfxFactory,
                            GfxContextCreateInfo const& gfxCtxInfo,
                            std::optional<Size2Du16> const clientAreaSize,
                            WindowMode const mode, bool const isUserResizeable)
  -> Win32AppWindowPtr {
  static auto const WINDOW_CLASS_ATOM = [&] {
    auto constexpr className = L"BasaltWindow";
    auto const bigIcon =
      win32::load_big_icon(nullptr, IDI_APPLICATION, LR_SHARED);
    auto const smallIcon =
      win32::load_small_icon(nullptr, IDI_APPLICATION, LR_SHARED);
    auto const backgroundBrush = GetSysColorBrush(COLOR_WINDOW);
    auto const windowClass = WNDCLASSEXW{
      sizeof(WNDCLASSEXW),
      0, // style
      &handle_create_message,
      0, // cbClsExtra
      0, // cbWndExtra
      moduleHandle,
      bigIcon,
      nullptr, // hCursor
      backgroundBrush, // TODO: is the background brush needed?
      nullptr, // lpszMenuName
      className,
      smallIcon,
    };

    auto const atom = RegisterClassExW(&windowClass);
    if (!atom) {
      BASALT_LOG_FATAL(create_win32_error_message(GetLastError()));
      BASALT_CRASH("Failed to register app window class");
    }

    return atom;
  }();

  // always create the window as windowed first to store the correct placement
  // when switching modes
  auto const [style, styleEx] = get_style_windowed(isUserResizeable);
  auto params = CreateParams{clientAreaSize};

  auto const handle =
    CreateWindowExW(styleEx, reinterpret_cast<LPCWSTR>(WINDOW_CLASS_ATOM),
                    title.c_str(), style, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
                    nullptr, nullptr, moduleHandle, &params);
  if (!handle) {
    BASALT_LOG_FATAL(create_win32_error_message(GetLastError()));
    BASALT_CRASH("Failed to create app window");
  }

  // allocate window object dynamically to keep its address stable because it is
  // stored as HWND user data
  auto window = std::make_unique<Win32AppWindow>(handle, messageQueue,
                                                 std::move(gfxFactory));

  window->set_mode(mode);
  window->init_gfx_context(gfxCtxInfo);

  return window;
}

Win32AppWindow::Win32AppWindow(HWND const handle,
                               Win32MessageQueue* messageQueue,
                               gfx::Win32GfxFactoryPtr gfxFactory)
  : Win32Window{handle, messageQueue}
  , mGfxFactory{std::move(gfxFactory)} {
  BASALT_ASSERT(mGfxFactory);

  // safe because the OS window data is destroyed by the destructor and moving
  // window objects is prohibited
  SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

  // replace bootstrap proc
  SetWindowLongPtrW(handle, GWLP_WNDPROC,
                    reinterpret_cast<LONG_PTR>(&route_message));
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

auto Win32AppWindow::init_gfx_context(GfxContextCreateInfo const& createInfo)
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
    mGfxFactory->create_context(handle(), createInfo.adapter, swapChainInfo);
  mSwapChain = mGfxContext->swap_chain();
  mExclusiveDisplayMode = createInfo.exclusiveDisplayMode;
}

auto Win32AppWindow::make_fullscreen() -> void {
  if (is_fullscreen()) {
    return;
  }

  mMode = WindowMode::Fullscreen;

  {
    auto const style = GetWindowLongW(handle(), GWL_STYLE);
    mSavedWindowInfo.style = static_cast<DWORD>(style & WS_OVERLAPPEDWINDOW);
    GetWindowRect(handle(), &mSavedWindowInfo.windowRect);

    auto const newStyle = style & ~WS_OVERLAPPEDWINDOW;
    SetWindowLongW(handle(), GWL_STYLE, newStyle);
  }

  auto const monitorInfo = [&] {
    auto const monitor = MonitorFromWindow(handle(), MONITOR_DEFAULTTONEAREST);

    auto info = MONITORINFO{};
    info.cbSize = sizeof(info);
    GetMonitorInfoW(monitor, &info);

    return info;
  }();
  auto const& windowRect = monitorInfo.rcMonitor;

  // SWP_NOCOPYBITS causes the window to flash white
  auto constexpr swpFlags =
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

  {
    auto const style = GetWindowLongW(handle(), GWL_STYLE);
    auto const newStyle = static_cast<LONG>(style | mSavedWindowInfo.style);
    SetWindowLongW(handle(), GWL_STYLE, newStyle);
  }
  {
    // HACK: update icon when switching to windowed because otherwise the icon
    // doesn't show up in the title bar when the window was initially shown as
    // fullscreen
    auto const bigIcon =
      reinterpret_cast<HICON>(GetClassLongPtrW(handle(), GCLP_HICON));
    auto const smallIcon =
      reinterpret_cast<HICON>(GetClassLongPtrW(handle(), GCLP_HICONSM));
    SendMessageW(handle(), WM_SETICON, ICON_BIG,
                 reinterpret_cast<LPARAM>(bigIcon));
    SendMessageW(handle(), WM_SETICON, ICON_SMALL,
                 reinterpret_cast<LPARAM>(smallIcon));
  }

  auto const& windowRect = mSavedWindowInfo.windowRect;

  // SWP_NOCOPYBITS causes the window to flash white
  auto constexpr swpFlags =
    UINT{SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED};
  SetWindowPos(handle(), nullptr, windowRect.left, windowRect.top,
               windowRect.right - windowRect.left,
               windowRect.bottom - windowRect.top, swpFlags);
}

auto Win32AppWindow::handle_message(UINT const messageId, WPARAM const wParam,
                                    LPARAM const lParam) -> LRESULT {
  switch (messageId) {
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

  return Win32Window::handle_message(messageId, wParam, lParam);
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

auto Win32AppWindow::route_message(HWND const handle, UINT const messageId,
                                   WPARAM const wParam, LPARAM const lParam)
  -> LRESULT {
  auto* const window = [&] {
    auto const userData = GetWindowLongPtrW(handle, GWLP_USERDATA);
    return reinterpret_cast<Win32AppWindow*>(userData);
  }();
  BASALT_ASSERT(window);

  return window->handle_message(messageId, wParam, lParam);
}

} // namespace basalt

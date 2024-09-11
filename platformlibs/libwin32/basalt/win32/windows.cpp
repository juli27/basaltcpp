#include "window.h"
#include "window_class.h"

#include "key_map.h"
#include "message_queue.h"
#include "util.h"

#include <basalt/win32/shared/utils.h>

#include <basalt/api/shared/size2d.h>
#include <basalt/api/shared/types.h>

#include <basalt/api/base/asserts.h>
#include <basalt/api/base/types.h>

#include <basalt/win32/shared/Windows_custom.h>
#include <windowsx.h>

#include <algorithm>
#include <array>
#include <memory>
#include <string_view>
#include <utility>

namespace basalt {

using std::array;
using std::wstring_view;

namespace {

auto virtual_key_code_to_key(UINT const virtualKeyCode, bool const isExtended)
  -> Key {
  if (virtualKeyCode == VK_RETURN && isExtended) {
    return Key::NumpadEnter;
  }

  return VK_TO_KEY_MAP[virtualKeyCode];
}

} // namespace

Win32Window::Win32Window(HWND const handle, Win32WindowClassCPtr clazz,
                         Win32MessageQueuePtr messageQueue)
  : mMessageQueue{std::move(messageQueue)}
  , mClass{std::move(clazz)}
  , mHandle{handle}
  , mMouseCursor{LoadCursorW(nullptr, IDC_ARROW)} {
  BASALT_ASSERT(mMessageQueue);
  BASALT_ASSERT(mClass);
  BASALT_ASSERT(mHandle);
  BASALT_ASSERT(mMouseCursor);

  // safe because the OS window data is destroyed by the destructor and moving
  // window objects is prohibited
  SetWindowLongPtrW(mHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

  // replace bootstrap proc
  SetWindowLongPtrW(mHandle, GWLP_WNDPROC,
                    reinterpret_cast<LONG_PTR>(&wnd_proc));
}

Win32Window::~Win32Window() noexcept {
  VERIFY_WIN32_BOOL(DestroyWindow(mHandle));
}

auto Win32Window::message_queue() const noexcept
  -> Win32MessageQueuePtr const& {
  return mMessageQueue;
}

auto Win32Window::clazz() const noexcept -> Win32WindowClassCPtr const& {
  return mClass;
}

auto Win32Window::handle() const noexcept -> HWND {
  return mHandle;
}

auto Win32Window::client_area_size() const noexcept -> Size2Du16 {
  auto clientRect = RECT{};
  VERIFY_WIN32_BOOL(GetClientRect(mHandle, &clientRect));

  return Size2Du16{static_cast<u16>(clientRect.right),
                   static_cast<u16>(clientRect.bottom)};
}

auto Win32Window::mouse_cursor() const noexcept -> HCURSOR {
  return mMouseCursor;
}

// issue with our input handling
//   WM_MOUSEMOVE is handled before WM_SETCURSOR and put in our
//   input queue -> the rest of the app gets the ability to set the
//   cursor only in the next frame -> the new cursor is only set at the
//   next WM_MOUSEMOVE/WM_SETCURSOR
auto Win32Window::set_mouse_cursor(HCURSOR const cursor) noexcept -> void {
  mMouseCursor = cursor;

  SetCursor(mMouseCursor);
}

auto Win32Window::input_manager() noexcept -> InputManager& {
  return mInputManager;
}

auto Win32Window::handle_message(UINT const message, WPARAM const wParam,
                                 LPARAM const lParam) -> LRESULT {
  switch (message) {
  case WM_SETCURSOR:
    return on_set_cursor(reinterpret_cast<HWND>(wParam), LOWORD(lParam),
                         HIWORD(lParam));

  case WM_SETFOCUS:
    return on_keyboard_focus_gained(reinterpret_cast<HWND>(wParam));

  case WM_KILLFOCUS:
    return on_keyboard_focus_lost(reinterpret_cast<HWND>(wParam));

  case WM_KEYDOWN:
    return on_key_down(static_cast<UINT>(wParam), LOWORD(lParam),
                       HIWORD(lParam));

  case WM_KEYUP:
    return on_key_up(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));

  case WM_CHAR:
    return on_char(static_cast<WCHAR>(wParam), LOWORD(lParam), HIWORD(lParam));

  case WM_MOUSEMOVE:
    return on_mouse_move(GET_KEYSTATE_WPARAM(wParam), MAKEPOINTS(lParam));

  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_RBUTTONDOWN:
  case WM_RBUTTONUP:
  case WM_MBUTTONDOWN:
  case WM_MBUTTONUP:
    return on_mouse_button(GET_KEYSTATE_WPARAM(wParam), MAKEPOINTS(lParam));

  case WM_XBUTTONDOWN:
  case WM_XBUTTONUP:
    return on_x_mouse_button(GET_KEYSTATE_WPARAM(wParam), MAKEPOINTS(lParam));

  case WM_MOUSEWHEEL: {
    return on_mouse_wheel(GET_WHEEL_DELTA_WPARAM(wParam),
                          GET_KEYSTATE_WPARAM(wParam), MAKEPOINTS(lParam));
  }

  default:
    return DefWindowProcW(mHandle, message, wParam, lParam);
  }
}

auto Win32Window::on_set_cursor(HWND const windowUnderCursor,
                                WORD const hitTestResult, WORD triggerMessage)
  -> LRESULT {
  if (hitTestResult != HTCLIENT) {
    return DefWindowProcW(mHandle, WM_SETCURSOR,
                          reinterpret_cast<WPARAM>(windowUnderCursor),
                          MAKELONG(hitTestResult, triggerMessage));
  }

  SetCursor(mMouseCursor);

  return TRUE;
}

auto Win32Window::on_keyboard_focus_gained(HWND) -> LRESULT {
  mInputManager.keyboard_focus_gained();

  return 0;
}

auto Win32Window::on_keyboard_focus_lost(HWND) -> LRESULT {
  mInputManager.keyboard_focus_lost();

  return 0;
}

auto Win32Window::on_key_down(UINT const virtualKeyCode, WORD, WORD const info)
  -> LRESULT {
  if (info & KF_REPEAT) {
    return 0;
  }

  auto const key = virtual_key_code_to_key(virtualKeyCode, info & KF_EXTENDED);

  // HACK: AltGr sends Ctrl + right Alt keydown messages but only sends
  // a keyup message for right Alt
  if (key == Key::Control) {
    if (auto next = mMessageQueue->peek()) {
      if (next->message == WM_KEYDOWN) {
        if (next->wParam == VK_MENU && HIWORD(next->lParam) & KF_EXTENDED &&
            next->time == static_cast<DWORD>(GetMessageTime())) {
          // skip ctrl message
          return 0;
        }
      }
    }
  }

  mInputManager.key_down(key);

  return 0;
}

auto Win32Window::on_key_up(UINT const virtualKeyCode, WORD, WORD const info)
  -> LRESULT {
  auto const key = virtual_key_code_to_key(virtualKeyCode, info & KF_EXTENDED);
  mInputManager.key_up(key);

  return 0;
}

auto Win32Window::on_char(WCHAR const c, WORD const repeatCount, WORD)
  -> LRESULT {
  // TODO: filter control characters
  // TODO: handle supplementary plane characters
  //       two messages are posted. create_utf8_from_wide handles the
  //       surrogates individually as invalid characters
  // TODO: convert to utf-8 without allocating
  auto const typedChar = create_utf8_from_wide(wstring_view{&c, 1});
  BASALT_ASSERT(typedChar.size() <= 4);
  auto character = array<char, 4>{};
  std::copy_n(typedChar.begin(), std::min(typedChar.size(), 4ull),
              character.begin());

  for (auto repCount = LOWORD(repeatCount); repCount > 0; repCount--) {
    mInputManager.character_utf8(character);
  }

  return 0;
}

// we process all passed input data (wParam & lParam) for every message:
// "When mouse messages are posted faster than a thread can process them,
// the system discards all but the most recent mouse message"
// https://docs.microsoft.com/en-us/windows/win32/inputdev/about-mouse-input#mouse-messages
auto Win32Window::on_mouse_move(WORD const buttonStates,
                                POINTS const pointerPos) -> LRESULT {
  // insert the mouse moved last, because this is the most recent message
  process_mouse_button_states(buttonStates);
  mInputManager.mouse_moved(PointerPosition{pointerPos.x, pointerPos.y});

  return 0;
}

auto Win32Window::on_mouse_button(WORD const buttonStates,
                                  POINTS const pointerPos) -> LRESULT {
  mInputManager.mouse_moved(PointerPosition{pointerPos.x, pointerPos.y});
  process_mouse_button_states(buttonStates);

  return 0;
}

auto Win32Window::on_x_mouse_button(WORD buttonStates, POINTS pointerPos)
  -> LRESULT {
  mInputManager.mouse_moved(PointerPosition{pointerPos.x, pointerPos.y});
  process_mouse_button_states(buttonStates);

  return TRUE;
}

auto Win32Window::on_mouse_wheel(SHORT const delta, WORD const buttonStates,
                                 POINTS const pointerPosScreen) -> LRESULT {
  auto pointerPos = POINT{};
  POINTSTOPOINT(pointerPos, pointerPosScreen);
  ScreenToClient(mHandle, &pointerPos);

  mInputManager.mouse_moved(PointerPosition{pointerPos.x, pointerPos.y});
  process_mouse_button_states(buttonStates);
  auto const offset = static_cast<f32>(delta) / static_cast<f32>(WHEEL_DELTA);
  mInputManager.mouse_wheel(offset);

  return 0;
}

auto Win32Window::process_mouse_button_states(WORD buttonStates) -> void {
  if (buttonStates & MK_SHIFT) {
    mInputManager.key_down(Key::Shift);
  } else {
    mInputManager.key_up(Key::Shift);
  }
  if (buttonStates & MK_CONTROL) {
    mInputManager.key_down(Key::Control);
  } else {
    mInputManager.key_up(Key::Control);
  }
  if (buttonStates & MK_LBUTTON) {
    mInputManager.mouse_button_down(MouseButton::Left);
  } else {
    mInputManager.mouse_button_up(MouseButton::Left);
  }
  if (buttonStates & MK_RBUTTON) {
    mInputManager.mouse_button_down(MouseButton::Right);
  } else {
    mInputManager.mouse_button_up(MouseButton::Right);
  }
  if (buttonStates & MK_MBUTTON) {
    mInputManager.mouse_button_down(MouseButton::Middle);
  } else {
    mInputManager.mouse_button_up(MouseButton::Middle);
  }
  if (buttonStates & MK_XBUTTON1) {
    mInputManager.mouse_button_down(MouseButton::Button4);
  } else {
    mInputManager.mouse_button_up(MouseButton::Button4);
  }
  if (buttonStates & MK_XBUTTON2) {
    mInputManager.mouse_button_down(MouseButton::Button5);
  } else {
    mInputManager.mouse_button_up(MouseButton::Button5);
  }

  if (buttonStates) {
    SetCapture(mHandle);
  } else {
    // release capture if all buttons are up
    // TODO: assert on the return value
    ReleaseCapture();
  }
}

auto Win32Window::instance(HWND const handle) -> Win32Window* {
  auto const userData = GetWindowLongPtrW(handle, GWLP_USERDATA);

  return reinterpret_cast<Win32Window*>(userData);
}

auto Win32Window::wnd_proc(HWND const handle, UINT const message,
                           WPARAM const wParam, LPARAM const lParam)
  -> LRESULT {
  auto* const window = instance(handle);
  BASALT_ASSERT(window);

  return window->handle_message(message, wParam, lParam);
}

auto Win32WindowClass::register_class(WNDCLASSEXW const& windowClass)
  -> Win32WindowClassCPtr {
  auto const atom = RegisterClassExW(&windowClass);
  if (!atom) {
    return nullptr;
  }

  return std::make_shared<Win32WindowClass const>(windowClass.hInstance, atom);
}

Win32WindowClass::Win32WindowClass(HMODULE const moduleHandle,
                                   ATOM const atom) noexcept
  : mModuleHandle{moduleHandle}
  , mAtom{atom} {
  BASALT_ASSERT(moduleHandle);
  BASALT_ASSERT(atom);
}

Win32WindowClass::~Win32WindowClass() noexcept {
  VERIFY_WIN32_BOOL(
    UnregisterClassW(reinterpret_cast<const WCHAR*>(mAtom), mModuleHandle));
}

auto Win32WindowClass::create_window(LPCWSTR name, DWORD style, DWORD styleEx,
                                     int x, int y, int width, int height,
                                     LPVOID param) const -> HWND {
  return CreateWindowExW(styleEx, reinterpret_cast<LPCWSTR>(mAtom), name, style,
                         x, y, width, height, nullptr, nullptr, mModuleHandle,
                         param);
}

} // namespace basalt

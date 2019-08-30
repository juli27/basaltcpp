#include <basalt/platform/Platform.h>
#include "Win32Platform.h"

#include <algorithm> // for_each
#include <array>
#include <memory> // make_unique
#include <stdexcept>
#include <string>
#include <system_error> // system_category
#include <vector>

#include <fmt/format.h>

#include <basalt/common/Types.h>
#include "D3D9GfxContext.h"
#include "Win32APIHeader.h"
#include <basalt/platform/events/Event.h>
#include <basalt/platform/events/KeyEvents.h>
#include <basalt/platform/events/MouseEvents.h>
#include <basalt/platform/events/WindowEvents.h>

#include <basalt/Log.h>
#include <basalt/common/Asserts.h>

#include "Util.h"

namespace basalt::platform {

using ::std::array;
using ::std::runtime_error;
using ::std::string;
using ::std::string_view;
using ::std::system_error;
using ::std::vector;
using ::std::wstring;
using ::std::wstring_view;

namespace winapi {
namespace {

struct WindowData final {
  WindowData() noexcept = default;
  WindowData(const WindowData&) noexcept = default;
  WindowData(WindowData&&) noexcept = default;
  ~WindowData() noexcept = default;

  auto operator=(const WindowData&) noexcept -> WindowData& = default;
  auto operator=(WindowData&&) noexcept -> WindowData& = default;

  IGfxContext* mGfxContext = nullptr;
  std::string mTitle;
  math::Vec2i32 mClientAreaSize;
  WindowMode mMode = WindowMode::Windowed;
  bool mResizeable = false;
  bool mMinimized = false;
  bool mMaximized = false;
};

// TODO: syskeys
// TODO: left and right variants
// TODO: disable SUPER? (only in fullscreen?)
constexpr array<Key, 256> VK_TO_KEY_MAP = {
  /* unassigned             */ Key::Unknown,
  /* VK_LBUTTON             */ Key::Unknown,
  /* VK_RBUTTON             */ Key::Unknown,
  /* VK_CANCEL              */ Key::Unknown,
  /* VK_MBUTTON             */ Key::Unknown,
  /* VK_XBUTTON1            */ Key::Unknown,
  /* VK_XBUTTON2            */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* VK_BACK                */ Key::Backspace,
  /* VK_TAB                 */ Key::Tab,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* VK_CLEAR               */ Key::Unknown,
  /* VK_RETURN              */ Key::Enter,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* VK_SHIFT               */ Key::Shift,
  /* VK_CONTROL             */ Key::Control,
  /* VK_MENU                */ Key::Alt, // TODO
  /* VK_PAUSE               */ Key::Pause,
  /* VK_CAPITAL             */ Key::CapsLock,
  /* VK_KANA                */ Key::Unknown, /* VK_HANGEUL, VK_HANGUL */
  /* unassigned             */ Key::Unknown,
  /* VK_JUNJA               */ Key::Unknown,
  /* VK_FINAL               */ Key::Unknown,
  /* VK_HANJA               */ Key::Unknown, /* VK_KANJI */
  /* unassigned             */ Key::Unknown,
  /* VK_ESCAPE              */ Key::Escape,
  /* VK_CONVERT             */ Key::Unknown,
  /* VK_NONCONVERT          */ Key::Unknown,
  /* VK_ACCEPT              */ Key::Unknown,
  /* VK_MODECHANGE          */ Key::Unknown,
  /* VK_SPACE               */ Key::Space,
  /* VK_PRIOR               */ Key::PageUp,
  /* VK_NEXT                */ Key::PageDown,
  /* VK_END                 */ Key::End,
  /* VK_HOME                */ Key::Home,
  /* VK_LEFT                */ Key::LeftArrow,
  /* VK_UP                  */ Key::UpArrow,
  /* VK_RIGHT               */ Key::RightArrow,
  /* VK_DOWN                */ Key::DownArrow,
  /* VK_SELECT              */ Key::Unknown,
  /* VK_PRINT               */ Key::Unknown,
  /* VK_EXECUTE             */ Key::Unknown,
  /* VK_SNAPSHOT            */ Key::Print, // TODO
  /* VK_INSERT              */ Key::Insert,
  /* VK_DELETE              */ Key::Delete,
  /* VK_HELP                */ Key::Unknown,
  /* 0                      */ Key::Zero,
  /* 1                      */ Key::One,
  /* 2                      */ Key::Two,
  /* 3                      */ Key::Three,
  /* 4                      */ Key::Four,
  /* 5                      */ Key::Five,
  /* 6                      */ Key::Six,
  /* 7                      */ Key::Seven,
  /* 8                      */ Key::Eight,
  /* 9                      */ Key::Nine,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* A                      */ Key::A,
  /* B                      */ Key::B,
  /* C                      */ Key::C,
  /* D                      */ Key::D,
  /* E                      */ Key::E,
  /* F                      */ Key::F,
  /* G                      */ Key::G,
  /* H                      */ Key::H,
  /* I                      */ Key::I,
  /* J                      */ Key::J,
  /* K                      */ Key::K,
  /* L                      */ Key::L,
  /* M                      */ Key::M,
  /* N                      */ Key::N,
  /* O                      */ Key::O,
  /* P                      */ Key::P,
  /* Q                      */ Key::Q,
  /* R                      */ Key::R,
  /* S                      */ Key::S,
  /* T                      */ Key::T,
  /* U                      */ Key::U,
  /* V                      */ Key::V,
  /* W                      */ Key::W,
  /* X                      */ Key::X,
  /* Y                      */ Key::Y,
  /* Z                      */ Key::Z,
  /* VK_LWIN                */ Key::Super, // TODO
  /* VK_RWIN                */ Key::Super, // TODO
  /* VK_APPS                */ Key::Menu,
  /* reserved               */ Key::Unknown,
  /* VK_SLEEP               */ Key::Unknown,
  /* VK_NUMPAD0             */ Key::Numpad0,
  /* VK_NUMPAD1             */ Key::Numpad1,
  /* VK_NUMPAD2             */ Key::Numpad2,
  /* VK_NUMPAD3             */ Key::Numpad3,
  /* VK_NUMPAD4             */ Key::Numpad4,
  /* VK_NUMPAD5             */ Key::Numpad5,
  /* VK_NUMPAD6             */ Key::Numpad6,
  /* VK_NUMPAD7             */ Key::Numpad7,
  /* VK_NUMPAD8             */ Key::Numpad8,
  /* VK_NUMPAD9             */ Key::Numpad9,
  /* VK_MULTIPLY            */ Key::NumpadMul,
  /* VK_ADD                 */ Key::NumpadAdd,
  /* VK_SEPARATOR           */ Key::Unknown,
  /* VK_SUBTRACT            */ Key::NumpadSub,
  /* VK_DECIMAL             */ Key::NumpadDecimal,
  /* VK_DIVIDE              */ Key::NumpadDiv,
  /* VK_F1                  */ Key::F1,
  /* VK_F2                  */ Key::F2,
  /* VK_F3                  */ Key::F3,
  /* VK_F4                  */ Key::F4,
  /* VK_F5                  */ Key::F5,
  /* VK_F6                  */ Key::F6,
  /* VK_F7                  */ Key::F7,
  /* VK_F8                  */ Key::F8,
  /* VK_F9                  */ Key::F9,
  /* VK_F10                 */ Key::F10, // TODO
  /* VK_F11                 */ Key::F11,
  /* VK_F12                 */ Key::F12, // TODO
  /* VK_F13                 */ Key::Unknown,
  /* VK_F14                 */ Key::Unknown,
  /* VK_F15                 */ Key::Unknown,
  /* VK_F16                 */ Key::Unknown,
  /* VK_F17                 */ Key::Unknown,
  /* VK_F18                 */ Key::Unknown,
  /* VK_F19                 */ Key::Unknown,
  /* VK_F20                 */ Key::Unknown,
  /* VK_F21                 */ Key::Unknown,
  /* VK_F22                 */ Key::Unknown,
  /* VK_F23                 */ Key::Unknown,
  /* VK_F24                 */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* VK_NUMLOCK             */ Key::NumpadLock,
  /* VK_SCROLL              */ Key::ScrollLock,
  /* VK_OEM_NEC_EQUAL       */ Key::Unknown, /* VK_OEM_FJ_JISHO */
  /* VK_OEM_FJ_MASSHOU      */ Key::Unknown,
  /* VK_OEM_FJ_TOUROKU      */ Key::Unknown,
  /* VK_OEM_FJ_LOYA         */ Key::Unknown,
  /* VK_OEM_FJ_ROYA         */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* VK_LSHIFT              */ Key::Unknown,
  /* VK_RSHIFT              */ Key::Unknown,
  /* VK_LCONTROL            */ Key::Unknown,
  /* VK_RCONTROL            */ Key::Unknown,
  /* VK_LMENU               */ Key::Unknown,
  /* VK_RMENU               */ Key::Unknown,
  /* VK_BROWSER_BACK        */ Key::Unknown,
  /* VK_BROWSER_FORWARD     */ Key::Unknown,
  /* VK_BROWSER_REFRESH     */ Key::Unknown,
  /* VK_BROWSER_STOP        */ Key::Unknown,
  /* VK_BROWSER_SEARCH      */ Key::Unknown,
  /* VK_BROWSER_FAVORITES   */ Key::Unknown,
  /* VK_BROWSER_HOME        */ Key::Unknown,
  /* VK_VOLUME_MUTE         */ Key::Unknown,
  /* VK_VOLUME_DOWN         */ Key::Unknown,
  /* VK_VOLUME_UP           */ Key::Unknown,
  /* VK_MEDIA_NEXT_TRACK    */ Key::Unknown,
  /* VK_MEDIA_PREV_TRACK    */ Key::Unknown,
  /* VK_MEDIA_STOP          */ Key::Unknown,
  /* VK_MEDIA_PLAY_PAUSE    */ Key::Unknown,
  /* VK_LAUNCH_MAIL         */ Key::Unknown,
  /* VK_LAUNCH_MEDIA_SELECT */ Key::Unknown,
  /* VK_LAUNCH_APP1         */ Key::Unknown,
  /* VK_LAUNCH_APP2         */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* VK_OEM_1               */ Key::Oem1,
  /* VK_OEM_PLUS            */ Key::Plus,
  /* VK_OEM_COMMA           */ Key::Comma,
  /* VK_OEM_MINUS           */ Key::Minus,
  /* VK_OEM_PERIOD          */ Key::Period,
  /* VK_OEM_2               */ Key::Oem2,
  /* VK_OEM_3               */ Key::Oem3,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* reserved               */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* VK_OEM_4               */ Key::Oem4,
  /* VK_OEM_5               */ Key::Oem5,
  /* VK_OEM_6               */ Key::Oem6,
  /* VK_OEM_7               */ Key::Oem7,
  /* VK_OEM_8               */ Key::Oem8,
  /* reserved               */ Key::Unknown,
  /* VK_OEM_AX              */ Key::Unknown,
  /* VK_OEM_102             */ Key::Oem9,
  /* VK_ICO_HELP            */ Key::Unknown,
  /* VK_ICO_00              */ Key::Unknown,
  /* VK_PROCESSKEY          */ Key::Unknown,
  /* VK_ICO_CLEAR           */ Key::Unknown,
  /* VK_PACKET              */ Key::Unknown,
  /* unassigned             */ Key::Unknown,
  /* VK_OEM_RESET           */ Key::Unknown,
  /* VK_OEM_JUMP            */ Key::Unknown,
  /* VK_OEM_PA1             */ Key::Unknown,
  /* VK_OEM_PA2             */ Key::Unknown,
  /* VK_OEM_PA3             */ Key::Unknown,
  /* VK_OEM_WSCTRL          */ Key::Unknown,
  /* VK_OEM_CUSEL           */ Key::Unknown,
  /* VK_OEM_ATTN            */ Key::Unknown,
  /* VK_OEM_FINISH          */ Key::Unknown,
  /* VK_OEM_COPY            */ Key::Unknown,
  /* VK_OEM_AUTO            */ Key::Unknown,
  /* VK_OEM_ENLW            */ Key::Unknown,
  /* VK_OEM_BACKTAB         */ Key::Unknown,
  /* VK_ATTN                */ Key::Unknown,
  /* VK_CRSEL               */ Key::Unknown,
  /* VK_EXSEL               */ Key::Unknown,
  /* VK_EREOF               */ Key::Unknown,
  /* VK_PLAY                */ Key::Unknown,
  /* VK_ZOOM                */ Key::Unknown,
  /* VK_NONAME              */ Key::Unknown,
  /* VK_PA1                 */ Key::Unknown,
  /* VK_OEM_CLEAR           */ Key::Unknown,
  /* reserved               */ Key::Unknown
};
constexpr wstring_view WINDOW_CLASS_NAME = L"BS_WINDOW_CLASS";

string sPlatformName;
vector<string> sArgs;
vector<PlatformEventCallback> sEventListener;
HINSTANCE sInstance;
int sShowCommand;
HWND sWindowHandle;
WindowData sWindowInfo;

/**
 * \brief Creates an error description from a Windows API error code.
 *
 * \param errorCode Windows API error code.
 * \return description string of the error.
 */
auto create_winapi_error_message(const DWORD errorCode) noexcept -> string {
  WCHAR* buffer = nullptr;
  const auto numChars = ::FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
    | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errorCode, 0u,
    reinterpret_cast<WCHAR*>(&buffer), 0u, nullptr
  );

  if (numChars == 0u) {
    return "FormatMessageW failed";
  }

  // use numChars because the buffer is NOT null terminated
  const auto message = create_utf8_from_wide({buffer, numChars});

  ::LocalFree(buffer);

  return message;
}

/**
 * \brief Processes the windows command line string and populates the argv
 *        style vector returned by platform::GetArgs().
 *
 * No program name will be added to the array.
 *
 * \param commandLine the windows command line arguments.
 */
void process_args(const WCHAR* commandLine) {
  // check if the command line string is empty to avoid adding
  // the program name to the argument vector
  if (commandLine[0] == L'\0') {
    return;
  }

  auto argc = 0;
  auto** argv = ::CommandLineToArgvW(commandLine, &argc);
  if (argv == nullptr) {
    // no logging because the log might not be initialized yet
    return;
  }

  sArgs.reserve(argc);
  for (auto i = 0; i < argc; i++) {
    sArgs.push_back(create_utf8_from_wide(argv[i]));
  }

  ::LocalFree(argv);
}

void dispatch_platform_event(const Event& event) {
  std::for_each(
    sEventListener.cbegin(), sEventListener.cend(),
    [&event](const PlatformEventCallback& callback) {
      callback(event);
    }
  );
}

LRESULT CALLBACK window_proc(
  HWND window, const UINT message, const WPARAM wParam, const LPARAM lParam
) {
  static auto sIsMinimized = false;

  switch (message) {
  case WM_MOUSEMOVE:
    dispatch_platform_event(
      MouseMovedEvent({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)})
    );
    return 0;

  case WM_MOUSEWHEEL: {
    const auto offset =
      GET_WHEEL_DELTA_WPARAM(wParam) / static_cast<f32> (WHEEL_DELTA);
    dispatch_platform_event(MouseWheelScrolledEvent(offset));
    return 0;
  }


  case WM_LBUTTONDOWN:
    ::SetCapture(window);
    dispatch_platform_event(MouseButtonPressedEvent(MouseButton::Left));
    return 0;

  case WM_LBUTTONUP:
    if (!::ReleaseCapture()) {
      BS_ERROR(
        "Releasing mouse capture in WM_LBUTTONUP failed: {}",
        create_winapi_error_message(::GetLastError())
      );
    }
    dispatch_platform_event(MouseButtonReleasedEvent(MouseButton::Left));
    return 0;

  case WM_RBUTTONDOWN:
    dispatch_platform_event(MouseButtonPressedEvent(MouseButton::Right));
    return 0;

  case WM_RBUTTONUP:
    dispatch_platform_event(MouseButtonReleasedEvent(MouseButton::Right));
    return 0;

  case WM_MBUTTONDOWN:
    dispatch_platform_event(MouseButtonPressedEvent(MouseButton::Middle));
    return 0;

  case WM_MBUTTONUP:
    dispatch_platform_event(MouseButtonReleasedEvent(MouseButton::Middle));
    return 0;

  // TODO: XBUTTON4 and XBUTTON5

  case WM_KEYDOWN:
  case WM_KEYUP: {
    const auto vkCode = static_cast<u8>(wParam);
    auto keyCode = VK_TO_KEY_MAP[vkCode];
    if (vkCode == VK_RETURN && (HIWORD(lParam) & KF_EXTENDED)) {
      keyCode = Key::NumpadEnter;
    }

    if (message == WM_KEYDOWN) {
      // don't dispatch repeat events
      if (HIWORD(lParam) & KF_REPEAT) {
        return 0;
      }

      // HACK: AltGr sends Ctrl + right Alt keydown messages but only sends
      // a keyup message for right Alt
      if (keyCode == Key::Control) {
        const DWORD ctrlMessageTime = ::GetMessageTime();
        MSG next{};
        if (::PeekMessageW(&next, nullptr, 0u, 0u, PM_NOREMOVE)) {
          if (next.message == WM_KEYDOWN) {
            if (next.wParam == VK_MENU
              && (HIWORD(next.lParam) & KF_EXTENDED)
              && next.time == ctrlMessageTime) {
              // skip ctrl message
              return 0;
            }
          }
        }
      }

      dispatch_platform_event(KeyPressedEvent(keyCode));
    } else {
      dispatch_platform_event(KeyReleasedEvent(keyCode));
    }
    return 0;
  }

  case WM_CHAR: {
    const auto typedChar = create_utf8_from_wide(
      wstring(1, static_cast<WCHAR>(wParam))
    );

    string typedChars;
    auto repCount = LOWORD(lParam);
    for (; repCount > 0; repCount--) {
      typedChars.append(typedChar);
    }

    dispatch_platform_event(CharactersTyped(typedChars));
    return 0;
  }

  case WM_SETFOCUS:
    return 0;

  case WM_KILLFOCUS:
    if (sWindowInfo.mMode != WindowMode::Windowed) {
      BS_TRACE("Minimizing");
      ::ShowWindow(sWindowHandle, SW_MINIMIZE);
    }
    return 0;

  case WM_ENTERSIZEMOVE:
    return 0;

  case WM_EXITSIZEMOVE:
    dispatch_platform_event(WindowResizedEvent(sWindowInfo.mClientAreaSize));
    return 0;

  case WM_SIZE:
    switch (wParam) {
    case SIZE_MINIMIZED:
      sIsMinimized = true;
      dispatch_platform_event(WindowMinimizedEvent());
      break;
    case SIZE_RESTORED:
      if (sIsMinimized) {
        sIsMinimized = false;
        dispatch_platform_event(WindowRestoredEvent());
        return 0;
      }
    [[fallthrough]];
    case SIZE_MAXIMIZED:
      sWindowInfo.mClientAreaSize.Set(LOWORD(lParam), HIWORD(lParam));
      dispatch_platform_event(WindowResizedEvent(sWindowInfo.mClientAreaSize));
      return 0;

    default:
      return 0;
    }
    return 0;

  case WM_CLOSE:
    ::DestroyWindow(window);
    return 0;

  case WM_DESTROY:
    delete sWindowInfo.mGfxContext;
    sWindowInfo.mGfxContext = nullptr;
    sWindowHandle = nullptr;
    ::PostQuitMessage(0);
    return 0;

  default:
    break;
  }

  return ::DefWindowProcW(window, message, wParam, lParam);
}

void register_window_class() {
  const auto cursor = static_cast<HCURSOR>(::LoadImageW(
    nullptr, MAKEINTRESOURCEW(OCR_NORMAL), IMAGE_CURSOR, 0, 0,
    LR_DEFAULTSIZE | LR_SHARED
  ));
  if (!cursor) {
    BS_ERROR("failed to load cursor");
  }

  WNDCLASSEXW windowClass{
    sizeof(WNDCLASSEXW),
    CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
    &window_proc,
    0, // cbClsExtra
    0, // cbWndExtra
    sInstance,
    nullptr, // hIcon
    cursor,
    nullptr, // hbrBackground
    nullptr, // lpszMenuName
    WINDOW_CLASS_NAME.data(),
    nullptr // hIconSm
  };

  if (!::RegisterClassExW(&windowClass)) {
    throw system_error(
      ::GetLastError(), std::system_category(),
      "Failed to register window class"
    );
  }

  BS_DEBUG("window class registered");
}

void create_main_window(const Config& config) {
  register_window_class();

  sWindowInfo.mTitle = config.mWindow.mTitle;
  sWindowInfo.mClientAreaSize = config.mWindow.mSize;
  sWindowInfo.mMode = config.mWindow.mMode;
  sWindowInfo.mResizeable = config.mWindow.mResizeable;

  DWORD style = 0u;
  DWORD styleEx = WS_EX_APPWINDOW;
  if (config.mWindow.mMode == WindowMode::Windowed) {
    style |= WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

    if (config.mWindow.mResizeable) {
      style |= WS_MAXIMIZEBOX | WS_SIZEBOX;
    }
  } else {
    style |= WS_POPUP;
    styleEx |= WS_EX_TOPMOST;
  }

  int xPos = 0;
  int yPos = 0;
  int width = ::GetSystemMetrics(SM_CXSCREEN);
  int height = ::GetSystemMetrics(SM_CYSCREEN);

  if (config.mWindow.mMode == WindowMode::Windowed) {
    // calculate the window size for the given client area size
    // and center the window on the primary monitor
    RECT rect{0, 0, config.mWindow.mSize.GetX(), config.mWindow.mSize.GetY()};
    if (!::AdjustWindowRectEx(&rect, style, FALSE, styleEx)) {
      throw system_error(::GetLastError(), std::system_category());
    }

    width = static_cast<int>(rect.right - rect.left);
    height = static_cast<int>(rect.bottom - rect.top);
    xPos = ::GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
    yPos = ::GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;
  }

  BS_DEBUG("creating window at ({}, {}) with size ({}, {})", xPos, yPos, width, height);

  const auto windowTitle = create_wide_from_utf8(config.mWindow.mTitle);
  sWindowHandle = ::CreateWindowExW(
    styleEx, WINDOW_CLASS_NAME.data(), windowTitle.c_str(), style, 
    CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, sInstance,
    nullptr
  );
  if (!sWindowHandle) {
    throw runtime_error("failed to create window");
  }

  BS_INFO("window created");
  ::ShowWindow(sWindowHandle, sShowCommand);

  sWindowInfo.mGfxContext = new D3D9GfxContext(sWindowHandle);
}

void create_platform_name_string() {
  DWORD historical = 0u;
  const auto size = ::GetFileVersionInfoSizeExW(
    FILE_VER_GET_NEUTRAL, L"kernel32.dll", &historical
  );
  if (size == 0u) {
    BS_ERROR("{}", create_winapi_error_message(::GetLastError()));
    return;
  }

  const auto buffer = std::make_unique<std::byte[]>(size);
  if (!::GetFileVersionInfoExW(
    FILE_VER_GET_NEUTRAL, L"kernel32.dll", 0u, size, buffer.get()
  )) {
    return;
  }

  VS_FIXEDFILEINFO* versionInfo = nullptr;
  UINT versionInfoSize = 0u;
  if (!::VerQueryValueW(
    buffer.get(), L"\\", reinterpret_cast<void**>(&versionInfo),
    &versionInfoSize
  )) {
    return;
  }

  BS_ASSERT(versionInfoSize >= sizeof(VS_FIXEDFILEINFO), "");
  BS_ASSERT(versionInfo, "");

  BOOL isWow64 = FALSE;
  if (!::IsWow64Process(::GetCurrentProcess(), &isWow64)) {
    BS_INFO("{}", create_winapi_error_message(::GetLastError()));
  }

  sPlatformName = fmt::format(
    "Windows API ({}.{}.{}{})",
    HIWORD(versionInfo->dwFileVersionMS), LOWORD(versionInfo->dwFileVersionMS),
    HIWORD(versionInfo->dwFileVersionLS), isWow64 ? " WOW64" : ""
  );
}

} // namespace

void init(HINSTANCE instance, const WCHAR* commandLine, const int showCommand) {
  sInstance = instance;
  sShowCommand = showCommand;

  process_args(commandLine);
  create_platform_name_string();
}

} // namespace winapi

void startup(const Config& config) {
  BS_ASSERT(winapi::sInstance, "Windows API not initialized");

  winapi::create_main_window(config);
}

void shutdown() {
  if (winapi::sWindowHandle) {
    ::DestroyWindow(winapi::sWindowHandle);
    winapi::sWindowHandle = nullptr;
  }

  if (!::UnregisterClassW(
    winapi::WINDOW_CLASS_NAME.data(), winapi::sInstance
  )) {
    BS_ERROR(
      "failed to unregister window class: {}",
      winapi::create_winapi_error_message(::GetLastError())
    );
  }
}

void add_event_listener(const PlatformEventCallback& callback) {
  winapi::sEventListener.push_back(callback);
}

auto poll_events() -> bool {
  // TODO[threading]: check if main thread

  MSG msg{};
  while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
    ::TranslateMessage(&msg);
    ::DispatchMessageW(&msg);

    if (!msg.hwnd) {
      switch (msg.message) {
        case WM_QUIT:
          BS_DEBUG("Received WM_QUIT message");
          return false;

        default:
          // ‭275‬ is WM_TIMER
          // is received upon focus change
          BS_WARN("unhandled thread message: {}", msg.message);
          break;
      }
    }
  }

  return true;
}

auto wait_for_events() -> bool {
  BS_DEBUG("waiting for events...");

  MSG msg{};
  const auto ret = ::GetMessageW(&msg, nullptr, 0u, 0u);
  if (ret == -1) {
    BS_ERROR(winapi::create_winapi_error_message(::GetLastError()));
    return false;
  }

  // GetMessage retrieved WM_QUIT
  if (ret == 0) {
    return false;
  }

  ::TranslateMessage(&msg);
  ::DispatchMessageW(&msg);

  // handle any remainig messages in the queue
  return poll_events();
}

void request_quit() {
  // TODO: more of a request and less of a do quit

  ::PostQuitMessage(0);
}

auto get_name() -> std::string_view {
  return winapi::sPlatformName;
}

auto get_args() -> const std::vector<std::string>& {
  return winapi::sArgs;
}

auto get_window_size() -> math::Vec2i32 {
  return winapi::sWindowInfo.mClientAreaSize;
}

auto get_window_mode() -> WindowMode {
  return winapi::sWindowInfo.mMode;
}

void set_window_mode(WindowMode windowMode) {
  switch (windowMode) {
  case WindowMode::Windowed:
    break;
  case WindowMode::Fullscreen:
    break;
  case WindowMode::FullscreenExclusive:
    break;
  }
  BS_ERROR("platform::set_window_mode not implemented");
}

auto get_window_gfx_context() -> IGfxContext* {
  // TODO: assert window exists
  return winapi::sWindowInfo.mGfxContext;
}

} // namespace basalt::platform

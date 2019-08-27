#include <basalt/platform/Platform.h>
#include <basalt/platform/PlatformWindowsAPI.h>

#include <algorithm> // for_each
#include <array>
#include <limits>
#include <memory> // make_unique
#include <stdexcept>
#include <string>
#include <system_error> // system_category
#include <vector>

#include <fmt/format.h>

#include <basalt/common/Types.h>
#include <basalt/platform/D3D9GfxContext.h>
#include <basalt/platform/WindowsHeader.h>
#include <basalt/platform/events/Event.h>
#include <basalt/platform/events/KeyEvents.h>
#include <basalt/platform/events/MouseEvents.h>
#include <basalt/platform/events/WindowEvents.h>

#include <basalt/Log.h>
#include <basalt/common/Asserts.h>

namespace basalt::platform {

using ::std::array;
using ::std::numeric_limits;
using ::std::runtime_error;
using ::std::string;
using ::std::string_view;
using ::std::system_error;
using ::std::vector;
using ::std::wstring;
using ::std::wstring_view;

namespace winapi {
namespace {

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
WindowData sWindowData;

/**
 * \brief Converts a Windows API wide string to UTF-8.
 *
 * The converted string is returned in a new string object.
 * Fails with std::terminate when string storage allocation throws.
 *
 * \param src the wide string to convert.
 * \return the wide string converted to UTF-8.
 */
auto create_utf8_from_wide(const wstring_view src) noexcept -> string {
  // Don't use asserts/log because this function is used before the log
  // is initialized

  // Do NOT call CreateWinAPIErrorMessage in this function
  // because it uses this function and may cause an infinite loop

  // TODO: noexcept allocator and heap memory pool for strings

  // WideCharToMultiByte fails when size is 0
  if (src.empty()) {
    return {};
  }

  // use the size of the string view because the input string
  // can be non null-terminated
  if (src.size() > static_cast<uSize>(numeric_limits<int>::max())) {
    return "create_utf8_from_wide: string to convert is too large";
  }

  const auto srcSize = static_cast<int>(src.size());
  auto dstSize = ::WideCharToMultiByte(
    CP_UTF8, 0u, src.data(), srcSize, nullptr, 0, nullptr, nullptr
  );

  if (dstSize == 0) {
    return "WideCharToMultiByte returned 0";
  }

  string dst(dstSize, '\0');
  dstSize = ::WideCharToMultiByte(
    CP_UTF8, 0u, src.data(), srcSize, dst.data(), static_cast<int>(dst.size()),
    nullptr, nullptr
  );
  if (dstSize == 0) {
    return "WideCharToMultiByte returned 0";
  }

  return dst;
}

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
  static auto sIsSizing = false;
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

  case WM_UNICHAR:
    if (wParam == UNICODE_NOCHAR) {
      return TRUE;
    }
    return ::DefWindowProcW(window, message, wParam, lParam);

  case WM_ENTERSIZEMOVE:
    sIsSizing = true;
    return 0;

  case WM_EXITSIZEMOVE:
    sIsSizing = false;
    dispatch_platform_event(WindowResizedEvent(sWindowData.mSize));
    return 0;

  case WM_SIZE:
    switch (wParam) {
    case SIZE_MINIMIZED:
      sIsMinimized = true;
      dispatch_platform_event(WindowMinimizedEvent());
      break;
    case SIZE_RESTORED:
      if (sIsMinimized) {
      dispatch_platform_event(WindowRestoredEvent());
      }
    [[fallthrough]];
    case SIZE_MAXIMIZED:
      sWindowData.mSize.Set(LOWORD(lParam), HIWORD(lParam));
      if (!sIsSizing) {
        dispatch_platform_event(WindowResizedEvent(sWindowData.mSize));
      }
      return 0;

    default:
      return 0;
    }
    return 0;

  case WM_CLOSE:
    ::DestroyWindow(window);
    return 0;

  case WM_DESTROY:
    if (sWindowData.mGfxContext) {
      delete sWindowData.mGfxContext;
    }
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
  BS_DEBUG(
    "creating main window:\n"
    "  title: \"{}\",\n  width: {}, height: {}, "
    "fullscreen: {}, exclusive: {},\n  resizeable: {}",
    config.mWindow.mTitle, config.mWindow.mSize.GetX(), config.mWindow.mSize.GetY(),
    config.mWindow.mMode != WindowMode::Windowed,
    config.mWindow.mMode == WindowMode::FullscreenExclusive, config.mWindow.mResizeable
  );

  register_window_class();

  DWORD style = WS_SIZEBOX;
  DWORD styleEx = 0;
  if (config.mWindow.mMode == WindowMode::Windowed) {
    style |= WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
      | WS_SYSMENU;
  } else {
    style |= WS_POPUP;
    styleEx |= WS_EX_TOPMOST;
  }

  if (config.mWindow.mMode != WindowMode::Windowed || !config.mWindow.mResizeable) {
    style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
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
      throw runtime_error(create_winapi_error_message(GetLastError()));
    }

    width = static_cast<int>(rect.right - rect.left);
    height = static_cast<int>(rect.bottom - rect.top);
    xPos = ::GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
    yPos = ::GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;
  }

  const wstring windowTitle = CreateWideFromUTF8(config.mWindow.mTitle);

  sWindowHandle = ::CreateWindowExW(
    styleEx, WINDOW_CLASS_NAME.data(), windowTitle.c_str(), style, xPos, yPos,
    width, height, nullptr, nullptr, sInstance, nullptr
  );
  if (!sWindowHandle) {
    throw runtime_error("failed to create window");
  }

  sWindowData.mGfxContext = new D3D9GfxContext(sWindowHandle);
  sWindowData.mTitle = config.mWindow.mTitle;
  sWindowData.mSize.Set(width, height);
  sWindowData.mMode = config.mWindow.mMode;
  sWindowData.mResizeable = config.mWindow.mResizeable;

  BS_INFO("window created");

  ::ShowWindow(sWindowHandle, sShowCommand);
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


auto GetWindowHandle() -> HWND {
  BS_ASSERT(sWindowHandle, "no window present");

  return sWindowHandle;
}

auto CreateWideFromUTF8(const string_view src) -> wstring {
  if (src.empty()) {
    return {};
  }

  BS_ASSERT(
    src.size() <= static_cast<uSize>(numeric_limits<int>::max()),
    "string too large"
  );
  const int srcSize = static_cast<int>(src.size());
  const int size = ::MultiByteToWideChar(
    CP_UTF8, 0, src.data(), srcSize, nullptr, 0
  );

  if (size == 0) {
    throw runtime_error("MultiByteToWideChar failed");
  }

  wstring dest(size, '\0');
  if (::MultiByteToWideChar(
    CP_UTF8, 0, src.data(), srcSize, dest.data(),
    static_cast<int>(dest.size())
  ) == 0) {
    throw runtime_error("MultiByteToWideChar failed");
  }

  return dest;
}

} // namespace winapi

void startup(const Config& config) {
  BS_ASSERT(winapi::sInstance, "Windows API not initialized");

  winapi::create_main_window(config);
}

void Shutdown() {
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

void AddEventListener(PlatformEventCallback callback) {
  winapi::sEventListener.push_back(callback);
}

auto PollEvents() -> bool {
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

auto WaitForEvents() -> bool {
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
  return PollEvents();
}

void RequestQuit() {
  // TODO: more of a request and less of a do quit

  ::PostQuitMessage(0);
}

auto GetName() -> std::string_view {
  return winapi::sPlatformName;
}

auto GetArgs() -> const std::vector<std::string>& {
  return winapi::sArgs;
}

auto get_window_data() -> const WindowData& {
  return winapi::sWindowData;
}

} // namespace basalt::platform

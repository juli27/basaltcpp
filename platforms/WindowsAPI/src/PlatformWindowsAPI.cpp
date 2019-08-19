#include <basalt/platform/Platform.h>
#include <basalt/platform/PlatformWindowsAPI.h>

#include <algorithm>
#include <array>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include <fmt/format.h>

#include <basalt/Log.h>
#include <basalt/common/Asserts.h>
#include <basalt/platform/WindowsHeader.h>
#include <basalt/platform/events/KeyEvents.h>
#include <basalt/platform/events/MouseEvents.h>
#include <basalt/platform/events/WindowEvents.h>

namespace basalt::platform {
namespace winapi {
namespace {


std::string sPlatformName;
std::vector<std::string> s_args;
std::vector<PlatformEventCallback> s_eventListener;
HINSTANCE s_instance;
int s_showCommand;
constexpr std::wstring_view s_windowClassName = L"BS_WINDOW_CLASS";
HWND s_windowHandle;
WindowDesc s_windowDesc;


// TODO: syskeys
// TODO: left and right variants
// TODO: disable SUPER? (only in fullscreen?)
constexpr std::array<Key, 256> VK_TO_KEY_MAP = {
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


/**
 * \brief Converts a Windows API wide string to UTF-8.
 *
 * The converted string is returned in a new string object.
 * Fails with std::terminate when string storage allocation throws.
 *
 * \param source the wide string to convert.
 * \return the wide string converted to UTF-8.
 */
auto CreateUTF8FromWide(const std::wstring_view source) noexcept -> std::string {
  // Do NOT call CreateWinAPIErrorMessage in this function
  // because it uses this function and may cause an infinite loop

  // WideCharToMultiByte fails when size is 0
  if (source.empty()) {
    return std::string();
  }

  // use the size of the string view because the input string
  // can be non null-terminated
  BS_RELEASE_ASSERT(
    source.size() <= static_cast<std::size_t>(std::numeric_limits<int>::max()),
    "string too large"
  );
  const int sourceSize = static_cast<int>(source.size());
  int size = ::WideCharToMultiByte(
    CP_UTF8, 0u, source.data(), sourceSize, nullptr, 0, nullptr, nullptr
  );

  if (size == 0) {
    BS_ERROR(
      "WideCharToMultiByte failed to return the required size for "
      "a wide string of size {} with error code {}", source.size(),
      ::GetLastError()
    );
    return std::string();
  }

  std::string dest(size, '\0');
  size = ::WideCharToMultiByte(
    CP_UTF8, 0, source.data(), sourceSize, dest.data(),
    static_cast<int>(dest.size()), nullptr, nullptr
  );
  if (size == 0) {
    BS_ERROR(
      "WideCharToMultiByte failed to convert a wide string of size {} "
      "with error code {}", sourceSize, ::GetLastError()
    );
    return std::string();
  }

  return dest;
}


/**
 * \brief Creates an error description from a Windows API error code.
 *
 * \param errorCode Windows API error code.
 * \return description string of the error.
 */
auto CreateWinAPIErrorMessage(DWORD errorCode) noexcept -> std::string {
  WCHAR* buffer = nullptr;
  const DWORD numChars = ::FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
    | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errorCode, 0u,
    reinterpret_cast<WCHAR*>(&buffer), 0u, nullptr
  );

  if (numChars == 0u) {
    BS_ERROR(
      "FormatMessageW failed with error code {}. The error code "
      "to format was {}", ::GetLastError(), errorCode
    );
    return std::string();
  }

  // use numChars because the buffer is NOT null terminated
  const std::string message = CreateUTF8FromWide({buffer, numChars});

  ::LocalFree(buffer);

  return message;
}


/*
 * \brief Processes the windows command line string and populates the argv
 *        style vector returned by platform::GetArgs().
 *
 * No program name will be added to the array.
 *
 * \param cmdLine the windows command line arguments.
 */
void ProcessArgs(const WCHAR* commandLine) {
  // check if the command line string is empty to avoid adding
  // the program name to the argument vector
  if (commandLine[0] == L'\0') {
    return;
  }

  int argc = 0;
  WCHAR** argv = ::CommandLineToArgvW(commandLine, &argc);
  if (argv == nullptr) {
    BS_ERROR(
      "CommandLineToArgvW failed: {}",
      CreateWinAPIErrorMessage(::GetLastError())
    );
    return;
  }

  s_args.reserve(argc);
  for (int i = 0; i < argc; ++i) {
    s_args.push_back(CreateUTF8FromWide(argv[i]));
  }

  ::LocalFree(argv);
}


void DispatchPlatformEvent(const Event& event) {
  std::for_each(
    s_eventListener.cbegin(), s_eventListener.cend(),
    [&event](const PlatformEventCallback& callback) {
      callback(event);
    }
  );
}


LRESULT CALLBACK WindowProc(
  HWND window, UINT message, WPARAM wParam, LPARAM lParam
) {
  switch (message) {
  case WM_MOUSEMOVE: {
    DispatchPlatformEvent(
      MouseMovedEvent({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)})
    );
    break;
  }

  case WM_MOUSEWHEEL: {
    const auto offset =
      GET_WHEEL_DELTA_WPARAM(wParam) / static_cast<f32> (WHEEL_DELTA);
    BS_TRACE("WM_MOUSEWHEEL: offset {}", offset);
    DispatchPlatformEvent(MouseWheelScrolledEvent(offset));
    return 0;
  }


  case WM_LBUTTONDOWN:
    DispatchPlatformEvent(MouseButtonPressedEvent(MouseButton::Left));
    break;

  case WM_LBUTTONUP:
    DispatchPlatformEvent(MouseButtonReleasedEvent(MouseButton::Left));
    break;

  case WM_RBUTTONDOWN:
    DispatchPlatformEvent(MouseButtonPressedEvent(MouseButton::Right));
    break;

  case WM_RBUTTONUP:
    DispatchPlatformEvent(MouseButtonReleasedEvent(MouseButton::Right));
    break;

  case WM_MBUTTONDOWN:
    DispatchPlatformEvent(MouseButtonPressedEvent(MouseButton::Middle));
    break;

  case WM_MBUTTONUP:
    DispatchPlatformEvent(MouseButtonReleasedEvent(MouseButton::Middle));
    break;

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
        break;
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
              break;
            }
          }
        }
      }

      DispatchPlatformEvent(KeyPressedEvent(keyCode));
    } else {
      DispatchPlatformEvent(KeyReleasedEvent(keyCode));
    }
    break;
  }

  case WM_CHAR: {
    const auto typedChar = CreateUTF8FromWide(
      std::wstring(1, static_cast<WCHAR>(wParam))
    );

    std::string typedChars;
    auto repCount = LOWORD(lParam);
    for (; repCount > 0; repCount--) {
      typedChars.append(typedChar);
    }

    DispatchPlatformEvent(CharactersTyped(typedChars));
    break;
  }

  case WM_UNICHAR:
    if (wParam == UNICODE_NOCHAR) {
      return TRUE;
    }
    return ::DefWindowProcW(window, message, wParam, lParam);

  case WM_SIZE:
    switch (wParam) {
      case SIZE_RESTORED:
      case SIZE_MAXIMIZED:
        DispatchPlatformEvent(
          WindowResizedEvent({LOWORD(lParam), HIWORD(lParam)})
        );
        return 0;

      default:
        break;
    }
    break;

  case WM_CLOSE:
    ::DestroyWindow(window);
    break;

  case WM_DESTROY:
    s_windowHandle = nullptr;
    ::PostQuitMessage(0);
    break;

  default:
    return ::DefWindowProcW(window, message, wParam, lParam);
  }

  return 0;
}


void RegisterWindowClass() {
  BS_DEBUG("registering window class...");

  HCURSOR cursor = static_cast<HCURSOR>(::LoadImageW(
    nullptr, MAKEINTRESOURCEW(OCR_NORMAL), IMAGE_CURSOR, 0, 0,
    LR_DEFAULTSIZE | LR_SHARED
  ));
  if (!cursor) {
    BS_ERROR("failed to load cursor");
  }

  WNDCLASSEXW windowClass{
    sizeof(WNDCLASSEXW),
    CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
    &WindowProc,
    0, // cbClsExtra
    0, // cbWndExtra
    s_instance,
    nullptr, // hIcon
    cursor,
    nullptr, // hbrBackground
    nullptr, // lpszMenuName
    s_windowClassName.data(),
    nullptr // hIconSm
  };

  if (!::RegisterClassExW(&windowClass)) {
    throw std::runtime_error("failed to register window class");
  }

  BS_DEBUG("window class registered");
}


void CreateMainWindow(const WindowDesc& desc) {
  BS_DEBUG(
    "creating main window:\n"
    "  title: \"{}\",\n  width: {}, height: {}, "
    "fullscreen: {}, exclusive: {},\n  resizeable: {}",
    desc.mTitle, desc.mSize.GetX(), desc.mSize.GetY(),
    desc.mMode != WindowMode::Windowed,
    desc.mMode == WindowMode::FullscreenExclusive, desc.mResizeable
  );

  RegisterWindowClass();

  DWORD style = WS_SIZEBOX;
  DWORD styleEx = 0;
  if (desc.mMode == WindowMode::Windowed) {
    style |= WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
      | WS_SYSMENU;
  } else {
    style |= WS_POPUP;
    styleEx |= WS_EX_TOPMOST;
  }

  if (desc.mMode != WindowMode::Windowed || !desc.mResizeable) {
    style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
  }

  int xPos = 0;
  int yPos = 0;
  int width = ::GetSystemMetrics(SM_CXSCREEN);
  int height = ::GetSystemMetrics(SM_CYSCREEN);

  if (desc.mMode == WindowMode::Windowed) {
    // calculate the window size for the given client area size
    // and center the window on the primary monitor
    RECT rect{0, 0, desc.mSize.GetX(), desc.mSize.GetY()};
    if (!::AdjustWindowRectEx(&rect, style, FALSE, styleEx)) {
      throw std::runtime_error(CreateWinAPIErrorMessage(GetLastError()));
    }

    width = static_cast<int>(rect.right - rect.left);
    height = static_cast<int>(rect.bottom - rect.top);
    xPos = ::GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
    yPos = ::GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;
  }

  const std::wstring windowTitle = CreateWideFromUTF8(desc.mTitle);

  s_windowHandle = ::CreateWindowExW(
    styleEx, s_windowClassName.data(), windowTitle.c_str(), style, xPos, yPos,
    width, height, nullptr, nullptr, s_instance, nullptr
  );
  if (!s_windowHandle) {
    throw std::runtime_error("failed to create window");
  }

  s_windowDesc = desc;

  BS_INFO("created window");

  ::ShowWindow(s_windowHandle, s_showCommand);
}

void CreatePlatformNameString() {
  DWORD historical = 0u;
  const auto size = ::GetFileVersionInfoSizeExW(
    FILE_VER_GET_NEUTRAL, L"kernel32.dll", &historical
  );
  if (size == 0u) {
    BS_ERROR("{}", CreateWinAPIErrorMessage(::GetLastError()));
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

  BS_RELEASE_ASSERT(versionInfoSize >= sizeof(VS_FIXEDFILEINFO), "");
  BS_RELEASE_ASSERT(versionInfo, "");

  BOOL isWow64 = FALSE;
  if (!::IsWow64Process(::GetCurrentProcess(), &isWow64)) {
    BS_INFO("{}", CreateWinAPIErrorMessage(::GetLastError()));
  }

  sPlatformName = fmt::format(
    "Windows API ({}.{}.{}{})",
    HIWORD(versionInfo->dwFileVersionMS), LOWORD(versionInfo->dwFileVersionMS),
    HIWORD(versionInfo->dwFileVersionLS), isWow64 ? " WOW64" : ""
  );
}

} // namespace

void Init(HINSTANCE instance, const WCHAR* commandLine, int showCommand) {
  BS_ASSERT_ARG_NOT_NULL(instance);
  BS_ASSERT_ARG_NOT_NULL(commandLine);

  BS_ASSERT(!s_instance, "Windows API already initialized");

  s_instance = instance;
  s_showCommand = showCommand;

  ProcessArgs(commandLine);
  CreatePlatformNameString();

  BS_DEBUG("Windows API platform initialized");
}


auto GetWindowHandle() -> HWND {
  BS_ASSERT(s_windowHandle, "no window present");

  return s_windowHandle;
}


auto CreateWideFromUTF8(const std::string_view source) -> std::wstring {
  if (source.empty()) {
    return std::wstring();
  }

  BS_RELEASE_ASSERT(
    source.size() <= static_cast<std::size_t>(std::numeric_limits<int>::max()),
    "string too large"
  );
  const int sourceSize = static_cast<int>(source.size());
  const int size = ::MultiByteToWideChar(
    CP_UTF8, 0, source.data(), sourceSize, nullptr, 0
  );

  if (size == 0) {
    throw std::runtime_error("MultiByteToWideChar failed");
  }

  std::wstring dest(size, '\0');
  if (::MultiByteToWideChar(
    CP_UTF8, 0, source.data(), sourceSize, dest.data(),
    static_cast<int>(dest.size())
  ) == 0) {
    throw std::runtime_error("MultiByteToWideChar failed");
  }

  return dest;
}

} // namespace winapi


void Startup(const WindowDesc& desc) {
  BS_ASSERT(winapi::s_instance, "Windows API not initialized");

  winapi::CreateMainWindow(desc);
}


void Shutdown() {
  if (winapi::s_windowHandle) {
    ::DestroyWindow(winapi::s_windowHandle);
    winapi::s_windowHandle = nullptr;
  }

  if (!::UnregisterClassW(
    winapi::s_windowClassName.data(), winapi::s_instance
  )) {
    BS_ERROR(
      "failed to unregister window class: {}",
      winapi::CreateWinAPIErrorMessage(::GetLastError())
    );
  }
}


void AddEventListener(PlatformEventCallback callback) {
  winapi::s_eventListener.push_back(callback);
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


void RequestQuit() {
  // TODO: more of a request and less of a do quit

  ::PostQuitMessage(0);
}


auto GetName() -> std::string_view {
  return winapi::sPlatformName;
}


auto GetArgs() -> const std::vector<std::string>& {
  return winapi::s_args;
}


auto GetWindowDesc() -> const WindowDesc& {
  // TODO: check if window is initialized
  return winapi::s_windowDesc;
}

} // namespace basalt::platform

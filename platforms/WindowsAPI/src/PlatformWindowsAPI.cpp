#include <basalt/platform/Platform.h>
#include <basalt/platform/PlatformWindowsAPI.h>

#include <algorithm>
#include <unordered_map>
#include <vector>

#include <basalt/Log.h>
#include <basalt/common/Asserts.h>
#include <basalt/platform/WindowsHeader.h>
#include <basalt/platform/events/KeyEvents.h>
#include <basalt/platform/events/MouseEvents.h>
#include <basalt/platform/events/WindowEvents.h>

namespace basalt {
namespace platform {
namespace winapi {
namespace {

// TODO: fill in windows version info
constexpr std::string_view s_platformName = "Windows API";
std::vector<std::string> s_args;
std::vector<PlatformEventCallback> s_eventListener;
HINSTANCE s_instance;
int s_showCommand;
constexpr std::wstring_view s_windowClassName = L"BS_WINDOW_CLASS";
HWND s_windowHandle;
WindowDesc s_windowDesc;


// TODO: syskeys (commented)
// TODO: keys in WM_KEYDOWN MESSAGE: LALT RALT;
// TODO: disable SUPER? (only in fullscreen?)
std::unordered_map<i32, Key> s_keyMap = {
  {VK_F1, Key::F1}, {VK_F2, Key::F2}, {VK_F3, Key::F3},
  {VK_F4, Key::F4}, {VK_F5, Key::F5}, {VK_F6, Key::F6},
  {VK_F7, Key::F7}, {VK_F8, Key::F8}, {VK_F9, Key::F9},
  /*{VK_F10, Key::F10},*/ {VK_F11, Key::F11}, /*{VK_F12, Key::F12},*/
  {VK_ESCAPE, Key::ESCAPE},
  {VK_TAB, Key::TAB},
  {VK_CAPITAL, Key::CAPS_LOCK},
  {VK_LSHIFT, Key::SHIFT_LEFT}, {VK_RSHIFT, Key::SHIFT_RIGHT},
  //{VK_MENU, Key::ALT},
  {VK_LWIN, Key::SUPER_LEFT},
  {VK_RWIN, Key::SUPER_RIGHT},
  {VK_PAUSE, Key::PAUSE},
  {VK_INSERT, Key::INSERT},
  {VK_DELETE, Key::DELETE},
  {VK_HOME, Key::HOME},
  {VK_END, Key::END},
  {VK_PRIOR, Key::PAGE_UP},
  {VK_NEXT, Key::PAGE_DOWN},
  {VK_LEFT, Key::LEFT_ARROW},
  {VK_RIGHT, Key::RIGHT_ARROW},
  {VK_UP, Key::UP_ARROW},
  {VK_DOWN, Key::DOWN_ARROW},
  {VK_NUMPAD0, Key::NUM_0}, {VK_NUMPAD1, Key::NUM_1},
  {VK_NUMPAD2, Key::NUM_2}, {VK_NUMPAD3, Key::NUM_3},
  {VK_NUMPAD4, Key::NUM_4}, {VK_NUMPAD5, Key::NUM_5},
  {VK_NUMPAD6, Key::NUM_6}, {VK_NUMPAD7, Key::NUM_7},
  {VK_NUMPAD8, Key::NUM_8}, {VK_NUMPAD9, Key::NUM_9},
  {VK_ADD, Key::NUM_ADD}, {VK_SUBTRACT, Key::NUM_SUB},
  {VK_MULTIPLY, Key::NUM_MUL}, {VK_DIVIDE, Key::NUM_DIV},
  {VK_DECIMAL, Key::NUM_DECIMAL}, {VK_NUMLOCK, Key::NUM_LOCK},
  {0x30, Key::ROW_0}, {0x31, Key::ROW_1},
  {0x32, Key::ROW_2}, {0x33, Key::ROW_3},
  {0x34, Key::ROW_4}, {0x35, Key::ROW_5},
  {0x36, Key::ROW_6}, {0x37, Key::ROW_7},
  {0x38, Key::ROW_8}, {0x39, Key::ROW_9},
  {VK_BACK, Key::BACKSPACE},
  {VK_SPACE, Key::SPACE},
  {VK_RETURN, Key::ENTER},
  {VK_APPS, Key::MENU},
  {VK_SCROLL, Key::SCROLL_LOCK},
  /*{VK_SNAPSHOT, Key::PRINT},*/
  {VK_OEM_PLUS, Key::PLUS},
  {0x41, Key::A}, {0x42, Key::B}, {0x43, Key::C},
  {0x44, Key::D}, {0x45, Key::E}, {0x46, Key::F},
  {0x47, Key::G}, {0x48, Key::H}, {0x49, Key::I},
  {0x4A, Key::J}, {0x4B, Key::K}, {0x4C, Key::L},
  {0x4D, Key::M}, {0x4E, Key::N}, {0x4F, Key::O},
  {0x50, Key::P}, {0x51, Key::Q}, {0x52, Key::R},
  {0x53, Key::S}, {0x54, Key::T}, {0x55, Key::U},
  {0x56, Key::V}, {0x57, Key::W}, {0x58, Key::X},
  {0x59, Key::Y}, {0x5A, Key::Z},
  {VK_OEM_MINUS, Key::MINUS},
  {VK_OEM_COMMA, Key::COMMA},
  {VK_OEM_PERIOD, Key::PERIOD},
  {VK_LCONTROL, Key::CONTROL_LEFT},
  {VK_RCONTROL, Key::CONTROL_RIGHT},
  {VK_OEM_1, Key::OEM_1},
  {VK_OEM_2, Key::OEM_2},
  {VK_OEM_3, Key::OEM_3},
  {VK_OEM_4, Key::OEM_4},
  {VK_OEM_5, Key::OEM_5},
  {VK_OEM_6, Key::OEM_6},
  {VK_OEM_7, Key::OEM_7},
  {VK_OEM_102, Key::OEM_9}
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
std::string CreateUTF8FromWide(const std::wstring_view source) noexcept {
  // Do NOT call CreateWinAPIErrorMessage in this function
  // because it uses this function and may cause an infinite loop

  // WideCharToMultiByte fails when size is 0
  if (source.empty()) {
    return std::string();
  }

  // use the size of the string view because the input string
  // can be non null-terminated
  int size = ::WideCharToMultiByte(
    CP_UTF8, 0u, source.data(), source.size(), nullptr, 0, nullptr, nullptr
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
    CP_UTF8, 0, source.data(), source.size(), dest.data(), dest.size(), nullptr,
    nullptr
  );
  if (size == 0) {
    BS_ERROR(
      "WideCharToMultiByte failed to convert a wide string of size {} "
      "with error code {}", source.size(), ::GetLastError()
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
std::string CreateWinAPIErrorMessage(DWORD errorCode) noexcept {
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
  LRESULT result = 0;

  switch (message) {
    case WM_MOUSEMOVE: {
      MouseMovedEvent event({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
      DispatchPlatformEvent(event);
      break;
    }

    case WM_LBUTTONDOWN: {
      MouseButtonPressedEvent event(MouseButton::LEFT);
      DispatchPlatformEvent(event);
      break;
    }

    case WM_LBUTTONUP: {
      MouseButtonReleasedEvent event(MouseButton::LEFT);
      DispatchPlatformEvent(event);
      break;
    }

    case WM_RBUTTONDOWN: {
      MouseButtonPressedEvent event(MouseButton::RIGHT);
      DispatchPlatformEvent(event);
      break;
    }

    case WM_RBUTTONUP: {
      MouseButtonReleasedEvent event(MouseButton::RIGHT);
      DispatchPlatformEvent(event);
      break;
    }

    case WM_MBUTTONDOWN: {
      MouseButtonPressedEvent event(MouseButton::MIDDLE);
      DispatchPlatformEvent(event);
      break;
    }

    case WM_MBUTTONUP: {
      MouseButtonReleasedEvent event(MouseButton::MIDDLE);
      DispatchPlatformEvent(event);
      break;
    }

    // TODO: XBUTTON4 and XBUTTON5

    case WM_KEYDOWN:
    case WM_KEYUP: {
      // TODO: propagate or filter repeat events
      //       filter with (HIWORD(lParam) & KF_REPEAT)

      Key keyCode = s_keyMap[wParam];
      switch (wParam) {
        case VK_SHIFT: {
          UINT scanCode = HIWORD(lParam) & 0xff;
          UINT newVk = ::MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX);
          keyCode = s_keyMap[newVk];
          break;
        }

        case VK_CONTROL: {
          if (HIWORD(lParam) & KF_EXTENDED) {
            keyCode = Key::CONTROL_RIGHT;
          } else {
            keyCode = Key::CONTROL_LEFT;
          }
          break;
        }

        case VK_RETURN:
          if (HIWORD(lParam) & KF_EXTENDED) {
            keyCode = Key::NUM_ENTER;
          } else {
            keyCode = Key::ENTER;
          }
          break;

        default:
          break;
      }

      if (message == WM_KEYDOWN) {
        KeyPressedEvent event(keyCode, LOWORD(lParam));
        DispatchPlatformEvent(event);
      } else {
        KeyReleasedEvent event(s_keyMap[wParam]);
        DispatchPlatformEvent(event);
      }
      break;
    }

    case WM_CHAR: {
      const std::string typedChar = CreateUTF8FromWide(
        std::wstring(1, static_cast<WCHAR>(wParam))
      );
      std::string typedChars;
      WORD repCount = LOWORD(lParam);
      for (; repCount > 0; repCount--) {
        typedChars.append(typedChar);
      }
      CharactersTyped event(typedChars);
      DispatchPlatformEvent(event);
      break;
    }

    case WM_SIZE:
      switch (wParam) {
        case SIZE_RESTORED:
        case SIZE_MAXIMIZED: {
          WindowResizedEvent event({LOWORD(lParam), HIWORD(lParam)});
          DispatchPlatformEvent(event);
          return 0;
        }
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
      result = ::DefWindowProcW(window, message, wParam, lParam);
      break;
  }

  return result;
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
    "fullscreen: {}, borderless: {},\n  resizeable: {}",
    desc.title, desc.size.GetX(), desc.size.GetY(),
    desc.mode != WindowMode::WINDOWED,
    desc.mode == WindowMode::FULLSCREEN_BORDERLESS, desc.resizeable
  );

  RegisterWindowClass();

  DWORD style = WS_SIZEBOX;
  DWORD styleEx = 0;
  if (desc.mode == WindowMode::WINDOWED) {
    style |= WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
      | WS_SYSMENU;
  } else {
    style |= WS_POPUP;
    styleEx |= WS_EX_TOPMOST;
  }

  if (desc.mode != WindowMode::WINDOWED || !desc.resizeable) {
    style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
  }

  int xPos = 0;
  int yPos = 0;
  int width = ::GetSystemMetrics(SM_CXSCREEN);
  int height = ::GetSystemMetrics(SM_CYSCREEN);

  if (desc.mode == WindowMode::WINDOWED) {
    // calculate the window size for the given client area size
    // and center the window on the primary monitor
    RECT rect{0, 0, desc.size.GetX(), desc.size.GetY()};
    if (!::AdjustWindowRectEx(&rect, style, FALSE, styleEx)) {
      throw std::runtime_error(CreateWinAPIErrorMessage(GetLastError()));
    }

    width = static_cast<int>(rect.right - rect.left);
    height = static_cast<int>(rect.bottom - rect.top);
    xPos = ::GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
    yPos = ::GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;
  }

  const std::wstring windowTitle = CreateWideFromUTF8(desc.title);

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

} // namespace


void Init(HINSTANCE instance, const WCHAR* commandLine, int showCommand) {
  BS_ASSERT_ARG_NOT_NULL(instance);
  BS_ASSERT_ARG_NOT_NULL(commandLine);

  BS_ASSERT(!s_instance, "Windows API already initialized");

  s_instance = instance;
  s_showCommand = showCommand;
  ProcessArgs(commandLine);

  BS_DEBUG("Windows API platform initialized");
}


HWND GetWindowHandle() {
  // TODO: check for null
  return s_windowHandle;
}


std::wstring CreateWideFromUTF8(const std::string_view source) {
  if (source.empty()) {
    return std::wstring();
  }

  const int size = ::MultiByteToWideChar(
    CP_UTF8, 0, source.data(), source.size(), nullptr, 0
  );

  if (size == 0) {
    throw std::runtime_error("MultiByteToWideChar failed");
  }

  std::wstring dest(size, '\0');
  if (::MultiByteToWideChar(
    CP_UTF8, 0, source.data(), source.size(), dest.data(), dest.size()
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
    winapi::s_windowClassName.data(), platform::winapi::s_instance
  )) {
    BS_ERROR(
      "failed to unregister window class: {}",
      platform::winapi::CreateWinAPIErrorMessage(::GetLastError())
    );
  }
}


void AddEventListener(PlatformEventCallback callback) {
  winapi::s_eventListener.push_back(callback);
}


bool PollEvents() {
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


std::string_view GetName() {
  return winapi::s_platformName;
}


const std::vector<std::string>& GetArgs() {
  return winapi::s_args;
}


const WindowDesc& GetWindowDesc() {
  // TODO: check if window is initialized
  return winapi::s_windowDesc;
}

} // namespace platform
} // namespace basalt

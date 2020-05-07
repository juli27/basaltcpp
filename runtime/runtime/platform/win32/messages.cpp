#include "runtime/platform/win32/messages.h"

#include "runtime/platform/win32/util.h"

#include <fmt/format.h>

#include <array>

namespace basalt {
namespace {

constexpr std::array<std::string_view, 43u> MESSAGE_NAMES {
  "WM_NULL",
  "WM_CREATE",
  "WM_DESTROY",
  "WM_MOVE",
  "unknown(0x4)",
  "WM_SIZE",
  "WM_ACTIVATE",
  "WM_SETFOCUS",
  "WM_KILLFOCUS",
  "unknown(0x9)",
  "WM_ENABLE",
  "WM_SETREDRAW",
  "WM_SETTEXT",
  "WM_GETTEXT",
  "WM_GETTEXTLENGTH",
  "WM_PAINT",
  "WM_CLOSE",
  "WM_QUERYENDSESSION",
  "WM_QUIT",
  "WM_QUERYOPEN",
  "WM_ERASEBKGND",
  "WM_SYSCOLORCHANGE",
  "WM_ENDSESSION",
  "unknown(0x17)",
  "WM_SHOWWINDOW",
  "unknown(0x19)",
  "WM_SETTINGCHANGE",
  "WM_DEVMODECHANGE",
  "WM_ACTIVATEAPP",
  "WM_FONTCHANGE",
  "WM_TIMECHANGE",
  "WM_CANCELMODE",
  "WM_SETCURSOR",
  "WM_MOUSEACTIVATE",
  "WM_CHILDACTIVATE",
  "WM_QUEUESYNC",
  "WM_GETMINMAXINFO",
  "unknown(0x25)",
  "WM_PAINTICON",
  "WM_ICONERASEBKGND",
  "WM_NEXTDLGCTL",
  "unknown(0x29)",
  "WM_SPOOLERSTATUS",
};

std::string_view size_w_param_to_string(WPARAM wParam);
std::string_view wa_w_param_status_to_string(WORD status);
std::string_view sw_l_param_to_string(LPARAM lParam);
std::string_view ht_to_string(WORD hitTestResult);
std::string get_message_name_or_hex_id(UINT message);
std::string to_string(POINT point);

} // namespace

std::string message_to_string(
  const UINT message, const WPARAM wParam, const LPARAM lParam
) {
  switch (message) {
  case WM_NULL:
    break;

  case WM_CREATE: {
    auto* createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
    return fmt::format(
      "{} CREATESTRUCT: lpCreateParams={} hInstance={} hMenu={} hwndParent={} "
      "cy={} cx={} y={} x={} style={:#x} lpszName={} lpszClass={} dwExStyle={:#x}"
    , MESSAGE_NAMES[message], createStruct->lpCreateParams
    , fmt::ptr(createStruct->hInstance)
    , fmt::ptr(createStruct->hMenu)
    , fmt::ptr(createStruct->hwndParent)
    , createStruct->cy
    , createStruct->cx
    , createStruct->y
    , createStruct->x
    , createStruct->style
    , create_utf8_from_wide(createStruct->lpszName)
    , fmt::ptr(createStruct->lpszClass)
    , createStruct->dwExStyle);
  }

  case WM_MOVE:
    return fmt::format(
      "{} x={} y={}", MESSAGE_NAMES[message], LOWORD(lParam), HIWORD(lParam));

  case WM_SIZE:
    return fmt::format(
      "{} type={} width={} height={}", MESSAGE_NAMES[message]
    , size_w_param_to_string(wParam), LOWORD(lParam), HIWORD(lParam));

  case WM_ACTIVATE:
    return fmt::format(
      "{} status={} minimized={:#x} otherWnd={}", MESSAGE_NAMES[message]
    , wa_w_param_status_to_string(LOWORD(wParam)), HIWORD(wParam)
    , fmt::ptr(reinterpret_cast<HWND>(lParam)));

  case WM_SETFOCUS:
  case WM_KILLFOCUS:
    return fmt::format(
      "{} otherWnd={}", MESSAGE_NAMES[message]
    , fmt::ptr(reinterpret_cast<HWND>(wParam)));

  case WM_ENABLE:
    return fmt::format("{} enabled={}", MESSAGE_NAMES[message], wParam);

  case WM_SETREDRAW:
    return fmt::format("{} redraw={}", MESSAGE_NAMES[message], wParam);

  case WM_SETTEXT:
    return fmt::format("{} text={}", MESSAGE_NAMES[message]
    , create_utf8_from_wide(reinterpret_cast<const wchar_t*>(lParam)));

  case WM_GETTEXT:
    return fmt::format("{} n={} buffer={}", MESSAGE_NAMES[message], wParam
    , fmt::ptr(reinterpret_cast<const wchar_t*>(lParam)));

  case WM_QUERYENDSESSION:
    return fmt::format("{} flags={:#x}", MESSAGE_NAMES[message], lParam);

  case WM_QUIT:
    return fmt::format("{} exitCode={}", MESSAGE_NAMES[message], wParam);

  case WM_ERASEBKGND:
    return fmt::format("{} hdc={}", MESSAGE_NAMES[message]
    , fmt::ptr(reinterpret_cast<HDC>(wParam)));

  case WM_ENDSESSION:
    return fmt::format("{} ending={} flags={:#x}", MESSAGE_NAMES[message], wParam
    , lParam);

  case WM_SHOWWINDOW:
    return fmt::format("{} show={} reason={}", MESSAGE_NAMES[message], wParam
    , sw_l_param_to_string(lParam));

  case WM_SETTINGCHANGE:
    return fmt::format("{} wParam={:#x} lParamString={}", MESSAGE_NAMES[message]
    , wParam, create_utf8_from_wide(reinterpret_cast<const wchar_t*>(lParam)));

  case WM_DEVMODECHANGE:
    return fmt::format("{} deviceName={}", MESSAGE_NAMES[message]
    , create_utf8_from_wide(reinterpret_cast<const wchar_t*>(lParam)));

  case WM_ACTIVATEAPP:
    return fmt::format("{} activated={} threadId={}", MESSAGE_NAMES[message]
    , wParam, lParam);

  case WM_SETCURSOR:
    return fmt::format("{} wnd={} htResult={} triggerMsg={}"
    , MESSAGE_NAMES[message], fmt::ptr(reinterpret_cast<HWND>(wParam))
    , ht_to_string(LOWORD(lParam))
    , get_message_name_or_hex_id(HIWORD(lParam)));

  case WM_MOUSEACTIVATE:
    return fmt::format("{} tlParentWnd={} htResult={} triggerMsg={}"
    , MESSAGE_NAMES[message], fmt::ptr(reinterpret_cast<HWND>(wParam))
    , ht_to_string(LOWORD(lParam))
    , get_message_name_or_hex_id(HIWORD(lParam)));

  case WM_GETMINMAXINFO: {
    const auto* const minMaxInfo = reinterpret_cast<MINMAXINFO*>(lParam);
    return fmt::format("{} maxSize={} maxPos={} minTrackSize={} maxTrackSize={}"
    , MESSAGE_NAMES[message], to_string(minMaxInfo->ptMaxSize)
    , to_string(minMaxInfo->ptMaxPosition)
    , to_string(minMaxInfo->ptMinTrackSize)
    , to_string(minMaxInfo->ptMaxTrackSize));
  }

  case WM_PAINTICON:
  case WM_ICONERASEBKGND:
    return fmt::format("{} wParam={} lParam={}", MESSAGE_NAMES[message], wParam
    , lParam);

  case WM_NEXTDLGCTL:
    return fmt::format("{} wParam={} isHandle={}"
    , MESSAGE_NAMES[message], wParam, LOWORD(lParam));

  case WM_SPOOLERSTATUS:
    return fmt::format("{} wParam={:#x} numJobs={}"
    , MESSAGE_NAMES[message], wParam, LOWORD(lParam));

  case WM_DESTROY:
  case WM_GETTEXTLENGTH:
  case WM_PAINT:
  case WM_CLOSE:
  case WM_QUERYOPEN:
  case WM_SYSCOLORCHANGE:
  case WM_FONTCHANGE:
  case WM_TIMECHANGE:
  case WM_CANCELMODE:
  case WM_CHILDACTIVATE:
  case WM_QUEUESYNC:
  default:
    break;
  }

  if (message < MESSAGE_NAMES.size()) {
    return std::string {MESSAGE_NAMES[message]};
  }

  return fmt::format(
    "unknown({:#x}) wParam: {} lParam: {}", message, wParam, lParam);
}

namespace {

#define HANDLE_CASE(value) case value: return #value;

std::string_view size_w_param_to_string(const WPARAM wParam) {
  switch (wParam) {
  HANDLE_CASE(SIZE_RESTORED)
  HANDLE_CASE(SIZE_MINIMIZED)
  HANDLE_CASE(SIZE_MAXIMIZED)
  HANDLE_CASE(SIZE_MAXSHOW)
  HANDLE_CASE(SIZE_MAXHIDE)
  default:
    return "invalid";
  }
}

std::string_view wa_w_param_status_to_string(const WORD status) {
  switch (status) {
  HANDLE_CASE(WA_INACTIVE)
  HANDLE_CASE(WA_ACTIVE)
  HANDLE_CASE(WA_CLICKACTIVE)
  default:
    return "invalid";
  }
}

std::string_view sw_l_param_to_string(const LPARAM lParam) {
  switch (lParam) {
  case 0:
    return "ShowWindow";
  HANDLE_CASE(SW_PARENTCLOSING)
  HANDLE_CASE(SW_OTHERZOOM)
  HANDLE_CASE(SW_PARENTOPENING)
  HANDLE_CASE(SW_OTHERUNZOOM)
  default:
    return "invalid";
  }
}

std::string_view ht_to_string(const WORD hitTestResult) {
  switch (hitTestResult) {
  HANDLE_CASE(HTERROR)
  HANDLE_CASE(HTTRANSPARENT)
  HANDLE_CASE(HTNOWHERE)
  HANDLE_CASE(HTCLIENT)
  HANDLE_CASE(HTCAPTION)
  HANDLE_CASE(HTSYSMENU)
  HANDLE_CASE(HTSIZE)
  HANDLE_CASE(HTMENU)
  HANDLE_CASE(HTHSCROLL)
  HANDLE_CASE(HTVSCROLL)
  HANDLE_CASE(HTMINBUTTON)
  HANDLE_CASE(HTMAXBUTTON)
  HANDLE_CASE(HTLEFT)
  HANDLE_CASE(HTRIGHT)
  HANDLE_CASE(HTTOP)
  HANDLE_CASE(HTTOPLEFT)
  HANDLE_CASE(HTTOPRIGHT)
  HANDLE_CASE(HTBOTTOM)
  HANDLE_CASE(HTBOTTOMLEFT)
  HANDLE_CASE(HTBOTTOMRIGHT)
  HANDLE_CASE(HTBORDER)
  HANDLE_CASE(HTOBJECT)
  HANDLE_CASE(HTCLOSE)
  HANDLE_CASE(HTHELP)
  default:
    return "invalid";
  }
}

std::string get_message_name_or_hex_id(const UINT message) {
  if (message < MESSAGE_NAMES.size()) {
    return std::string {MESSAGE_NAMES[message]};
  }

  return fmt::format("{:#x}", message);
}

std::string to_string(const POINT point) {
  return fmt::format("{}x{}", point.x, point.y);
}

#undef HANDLE_CASE

} // namespace

} // namespace basalt
